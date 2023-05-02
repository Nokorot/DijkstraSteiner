#include "algorithm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fibheap.h"

#define abs(x) ((x < 0) ? -(x) : (x))
int RLcost(Point *a, Point *b) {
    return abs(a->x - b->x) + abs(a->y - b->y);
}

/* Possible Optimizations:
 *   - Assume t to be the last element so we need only consider I in 2^(|T|-1) in stead of 2^|T|
 */

#define NODES_COUNT(n,m) m * (1 << n)
#define SIZEOF_NODES(n,m) sizeof(Node) * NODES_COUNT(n,m)

typedef struct Node {
    int l;    // The value of l(v, I)

    bool defined; // This is set to true when the node is initialized.
                  // It means l(v,I) = infty. 
                  // Note that v and I are also not set, when defined=false

    uint8 v; // index of element in V(G).
    int I;   // in 2^T

    bool inP; // Is this 
    
    // Keeping track of the edges, used when 'untangeling' 
    // the minimal stainer tree
    int edgeCount;
    struct Node *connection; // InP
    struct Node *left;       // InP
    struct Node *right;      // InP

    // Each node also acts as a node in the FibHeap
    FH_Node fh_nd;
} Node;

typedef struct AlgData {
    uint8 n; // # Terminals
    uint8 m; // |V(G)|
    uint8 t; // fixed terminal

    Node *nodes;  // An array of nodes indexed by V(G)x2^T.

    // A priorety que, implemented as a FibHeap
    FibHeap *fh;

    Graph G; // The Graph
    int *T; // Terminals
} AlgData;


int sizeofNodes(int n, int m) {
    return sizeof(Node) * m * (1 << n);
}

void untangle(Graph G, Node *nd, Graph *mst, int ia, int *idx);
void stage7(const AlgData *dat, int v, int I,   int J, int k);


float compare_nodes(void *a, void *b) 
{
    return ((Node *) a)->l - ((Node *) b)->l;
}

void set_key(void *a, float key) 
{
    ((Node *)a)->l = key;
}

Node *getNode(const AlgData *dat, int v, int I) 
{
    return &dat->nodes[v + I*dat->m];
}

void set_l(const AlgData *dat, uint8 v, int I, int l) 
{
    Node *nd = getNode(dat, v, I);

    if (!nd->defined) {
        // *nd = (Node){ defined: true, v: v, I: I, l: l };
        nd->defined = true;
        nd->v = v;
        nd->I = I;
        nd->l = l;
    
        nd->fh_nd.data = nd;
        FH_insert(dat->fh, &nd->fh_nd);
    }
    else if (nd->l != l)
        FH_decrease_key(dat->fh, &(nd->fh_nd), l);
    else
        return;
}

void untangle(Graph G, Node *nd, Graph *mst, int ia, int *idx) 
{
    Point a = G.V[(size_t) nd->v];
    mst->V[ia].x = a.x;
    mst->V[ia].y = a.y;

    if (nd->connection) {
        int ib = ++*idx, in = 0; 
        while (mst->V[ia].neigbours[in]) in++;
        mst->V[ia].neigbours[in] = &mst->V[ib];
        
        untangle(G, nd->connection, mst, ib, idx);
    } else {
        if (nd->left)
            untangle(G, nd->left, mst, ia, idx);
        if (nd->right)
            untangle(G, nd->right, mst, ia, idx);
    }
}

int Algorithm(uint8 n, Graph G, int *T, uint8 t, Graph *mst, bool counter)
{
    AlgData dat = { n, G.size, t, NULL, NULL, G, T };
    // AlgData dat = { n, G.size, t, G: G, T };

    if (counter)
        printf("sizeof(nodes): %lu bytes\n", SIZEOF_NODES(n, dat.m));

    dat.nodes = malloc(SIZEOF_NODES(n, dat.m));
    memset(dat.nodes, 0, SIZEOF_NODES(n, dat.m)); 
    // Could almost just set all the values to l, INTMAX
                           // but this is likly faster
 
    // FibHeap fh = { size: 0, min_root: NULL, nr_roots: 0 };
    FibHeap fh = { 0 };
    fh.compare = &compare_nodes;
    fh.set_key = &set_key;

    fh.roots = malloc(sizeof(FH_Node *) * NODES_COUNT(n,dat.m));
    memset(fh.roots, 0, sizeof(FH_Node *) * NODES_COUNT(n,dat.m));
    
    dat.fh = &fh;

    // Stage 1:
    for (uint8 j=0; j<n; ++j)
        if (j != t)
            set_l(&dat, T[(size_t) j], 1 << j, 0);

    // Stage 2:
    // This step is unnessesary.
    
    int loop_counter = 0;

    // Stage 3:
    Node *nd, *_nd;
    for (;true; loop_counter++) {
        nd = FH_extract_min(dat.fh);
 
        if (counter  && loop_counter % 1000 == 0)
            printf("loop_counter: %5uk\n", loop_counter / 1000);
     
        // Assert(nd != NULL);

        // Stage 4:
        nd->inP = true;

        // Stage 5:
        if (nd->v == T[(size_t) t] && (nd->I ^ (1 << t)) == (1 << n) - 1) {
            int result = nd->l;
            if (mst) {
                int vs = nd->edgeCount + 1;
                *mst = (Graph) { malloc(sizeof(Point)*vs), vs };

                memset(mst->V, 0, sizeof(Point)*vs);
                int i = 0;
                untangle(dat.G, nd, mst, 0, &i);
            }
            free(dat.nodes);
            return result;
        }

        // Stage 6:
        for (int j=0; j<MAX_NEIGBOUR_COUNT; ++j) {
            Point *wp = G.V[(size_t) nd->v].neigbours[j];
            if (!wp) continue;
 
            int w = wp - G.V;

            int l = nd->l + RLcost(&G.V[(size_t) nd->v], &G.V[w]);

            _nd = getNode(&dat, w, nd->I);
            if (!_nd->defined || l < _nd->l) {
                _nd->connection = nd;
                _nd->edgeCount = nd->edgeCount+1;

                set_l(&dat, w, nd->I, l);
            }
        }

        // Stage 7:
        stage7(&dat, nd->v, nd->I, 0, 0);

        // Stage 8: Return to 3
    }

    free(dat.nodes);
    return -1;
}

// Itterating over all subsets of T\I, recursivly:
void stage7(const AlgData *dat, int v, int I,   int J, int k) 
{
    for (int j = k; j < dat->n; ++j) {
        if (I ^ (1 << j))
            stage7(dat, v, I,  J | (1 << j), j+1);
    }

    Node *nd_J = getNode(dat, v, J);
    if (!J || !nd_J->inP) 
        return;

    Node *nd_I = getNode(dat, v, I);
    Node *nd_IJ = getNode(dat, v, I|J);

    int new_l = nd_I->l + nd_J->l;
    if (!nd_IJ->defined || new_l < nd_IJ->l) {
        nd_IJ->connection = NULL;

        nd_IJ->left = nd_I;
        nd_IJ->right = nd_J;
        nd_IJ->edgeCount = nd_I->edgeCount + nd_J->edgeCount;
        set_l(dat,  v, I|J, new_l);
    }
}

