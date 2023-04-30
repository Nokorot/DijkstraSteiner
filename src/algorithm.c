#include "algorithm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define abs(x) ((x < 0) ? -(x) : (x))
int RLcost(Point *a, Point *b) {
    return abs(a->x - b->x) + abs(a->y - b->y);
}

/* Optimizations:
 *   - Replace the SSlist with a Fib heap
 *   - assume t to be the last element so we need only consider I in 2^(|T|-1) in stead of 2^|T|
 * */


#define SIZEOF_NODES(n,m) sizeof(Node) * m * (1 << n)

typedef struct Node {
    bool defined; // This is set to true when the node is initialized.
                  // It means l(v,I) = infty. 
                  // Note that v and I are also not set, when defined=false

    uint8 v; // index of element in V(G).
    int I;   // in 2^T

    int l;    // The value of l(v, I)
    bool inP; // Is this 
    
    // Keeping track of the edges
    int edgeCount;
    struct Node *connection; // InP
    struct Node *left;       // InP
    struct Node *right;      // InP

    // Priorety que for stage 3
    struct Node *next; // NotP
    struct Node *prev; // NotP
} Node;

typedef struct SLList {
    struct Node *first;
    // int size; Not implemented
} SLList;

typedef struct AlgData {
    uint8 n; // # Terminals
    uint8 m; // |V(G)|
    uint8 t; // fixed terminal

    Node *nodes;  // An array of nodes indexed by V(G)x2^T.
    SLList *notP; // A Sorted Linked List of nodes. A priorety que for stage 3 
    // Node *notP_first;

    Graph G; // The Graph
    int *T; // Terminals
} AlgData;


int sizeofNodes(int n, int m) {
    return sizeof(Node) * m * (1 << n);
}

void SLL_add(struct SLList *sll, Node *nd);
void SLL_drop(struct SLList *sll, Node *nd);
void untangle(Graph G, Node *nd, Graph *mst, int ia, int *idx);
void stage7(const AlgData *dat, int v, int I,   int J, int k);


// int Algorithm(uint8 n, Graph G, int *T, uint8 t) {


// Add node to sorted linked list. Note that is is linear in time
void SLL_add(struct SLList *sll, Node *nd) {
    if (!sll->first) {
        nd->prev = NULL; // This is not realy nessesary
        nd->next = NULL; // This is not realy nessesary
        sll->first = nd;
        return;
    }
    if (nd->l <= sll->first->l) {
        nd->prev = NULL; // This is not realy nessesary
        nd->next = sll->first;
        nd->next->prev = nd;
        sll->first = nd;
        return;
    }
    
    // IDEA: Could stor the cursor in SLL
    Node *cursor = sll->first;
    while (cursor->next && nd->l > cursor->next->l)
        cursor = cursor->next;
    
    nd->prev = cursor;
    nd->next = cursor->next;
    if (nd->next)
        nd->next->prev = nd;
    cursor->next = nd;
    return;
}

// Removes nd from the linked list.
void SLL_drop(struct SLList *sll, Node *nd) {
    if (sll->first == nd) {
        sll->first = nd->next;
        return;
    }

    if (nd->prev) 
        nd->prev->next = nd->next;
    if (nd->next)
        nd->next->prev = nd->prev;
    nd->prev = NULL;
    nd->next = NULL;
}

Node *getNode(const AlgData *dat, int v, int I) {
    return &dat->nodes[v + I*dat->m];
}

void set_l(const AlgData *dat, uint8 v, int I, int l) {
    Node *nd = getNode(dat, v, I);

    if (!nd->defined) {
        nd->defined = true;
    
        nd->v = v;
        nd->I = I;

        nd->l = l;
        nd->prev = NULL;
        nd->next = NULL;

        SLL_add(dat->notP, nd);
    } else if (nd->l != l && nd->prev) {
        SLL_drop(dat->notP, nd);
        nd->l = l;
        SLL_add(dat->notP, nd);
    } else {
        return;
    }
}
void untangle(Graph G, Node *nd, Graph *mst, int ia, int *idx) {
    Point a = G.V[nd->v];
    mst->V[ia].x = a.x;
    mst->V[ia].y = a.y;

    if (nd->connection) {
        Point b = G.V[nd->connection->v];
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

int Algorithm(uint8 n, Graph G, int *T, uint8 t, Graph *mst) {
    AlgData dat = { n: n, m: G.size, t: t, G: G, T: T };

    printf("n: %u, m: %u, sizeof(nodes): %u\n", n, dat.m, SIZEOF_NODES(n, dat.m));

    dat.nodes = malloc(SIZEOF_NODES(n, dat.m));
    memset(dat.nodes, 0, SIZEOF_NODES(n, dat.m)); 
    // Could almost just set all the values to l, INTMAX
                           // but this is likly faster
                           
    SLList notP = { first: NULL, /* size: 0, */ };
    dat.notP = &notP;

    // Stage 1:
    for (uint8 j=0; j<n; ++j)
        if (j != t)
            set_l(&dat, T[j], 1 << j, 0);

    // Stage 2:
    // This step is unnessesary.
    
    // Stage 3:
    Node *nd, *_nd;
    while (true) {

        nd = notP.first;
        // Assert(nd != NULL);
        
        // Stage 4:
        SLL_drop(&notP, nd);
        nd->inP = 1;

        // Stage 5:
        if (nd->v == T[t] && (nd->I ^ (1 << t)) == (1 << n) - 1) {
            int result = nd->l;
            if (mst) {
                int vs = nd->edgeCount + 1;
                *mst = (Graph) { V: malloc(sizeof(Point)*vs), vs };

                memset(mst->V, 0, sizeof(Point)*vs);
                int i = 0;
                untangle(dat.G, nd, mst, 0, &i);
            }
            free(dat.nodes);
            return result;
        }

        // Stage 6:
        for (int j=0; j<MAX_NEIGBOUR_COUNT; ++j) {
            Point *wp = G.V[nd->v].neigbours[j];
            if (!wp) continue;
 
            int w = wp - G.V;

            int l = nd->l + RLcost(&G.V[nd->v], &G.V[w]);

            _nd = getNode(&dat, w, nd->I);
            if (!_nd->defined || l < _nd->l) {
                _nd->connection = getNode(&dat, nd->v, nd->I);
                _nd->edgeCount = nd->edgeCount+1;

                set_l(&dat, w, nd->I, l);
            }
        }

        // Stage 7:
        stage7(&dat, nd->v, nd->I, 0, 0);

        // Stage 8: 
    }

    free(dat.nodes);
    return -1;
}

// This is recursive:
void stage7(const AlgData *dat, int v, int I,   int J, int k) {
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
        nd_IJ->left = nd_I;
        nd_IJ->right = nd_J;
        nd_IJ->edgeCount = nd_I->edgeCount + nd_J->edgeCount;
        set_l(dat,  v, I|J, new_l);
    }
}

