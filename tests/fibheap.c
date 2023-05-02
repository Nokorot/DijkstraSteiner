#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/fibheap.h"

typedef struct Node {
    float l;

    FH_Node fh_nd;
} Node;

float compare(void *a, void *b) {
    // printf("a: %f, b: %f\n", ((Node *)a)->l, ((Node *)b)->l);
    return ((Node *)a)->l - ((Node *) b)->l;
}

void set_key(void *a, float key) {
    ((Node *)a)->l = key;
}

void set_l(FibHeap *fh, Node *nd, float l) {
    nd->l = l;
    nd->fh_nd.data = nd;

    FH_insert(fh, &nd->fh_nd);
}


void print_fh(Node *nodes, FH_Node *nd_a, int indent) {
    // nd = fh.min_root;
    FH_Node *nd  = nd_a;
    do {
        Node *nn = ((Node *) nd->data);
        for (int i=0; i<indent; ++i)
            printf("- ");
        printf("%2u: %f, left: %u, m: %b dg: %u\n", nn - nodes, nn->l, ((Node *) nd->left) - nodes, nd->marked, nd->degree);
        if (nd->min_child)
            print_fh(nodes, nd->min_child, indent+1);
        nd = nd->right;
    } while (nd != nd_a);
}


int main(int argc, char **argv) {
    FibHeap fh = { size: 0, min_root: NULL };
    fh.compare = &compare;
    fh.set_key = &set_key;

    fh.roots = malloc(sizeof(FH_Node *)*100);
    memset(fh.roots, 0, sizeof(FH_Node *)*100);

    Node *nodes = malloc(sizeof(Node)*100);
    memset(nodes, 0, sizeof(Node *)*100);

    set_l(&fh, &nodes[1], 5.0);
    set_l(&fh, &nodes[2], 5.0);
    set_l(&fh, &nodes[3], 5.0);
    set_l(&fh, &nodes[4], 5.0);
    set_l(&fh, &nodes[5], 6.0);
    set_l(&fh, &nodes[6], 7.0);
    set_l(&fh, &nodes[7], 3.0);
    set_l(&fh, &nodes[8], 9.0);
    set_l(&fh, &nodes[9], 8.0);
    set_l(&fh, &nodes[10], 2.0);
    set_l(&fh, &nodes[11], 9.0);


    FH_Node *nd = fh.min_root;
    print_fh(nodes, nd, 0);

    printf("R1: %u\n", fh.nr_roots);


    Node *min = (Node *) FH_extract_min(&fh);
    printf("min: %u %f\n\n", min - nodes, min->l);
    nd = fh.min_root;
    print_fh(nodes, nd, 0);

    min = (Node *) FH_extract_min(&fh);
    printf("min: %u %f\n", min - nodes, min->l);
    nd = fh.min_root;
    print_fh(nodes, nd, 0);

    min = (Node *) FH_extract_min(&fh);
    printf("Amin: %u %f\n", min - nodes, min->l);
    nd = fh.min_root;
    print_fh(nodes, nd, 0);


    FH_decrease_key(&fh, &nodes[8].fh_nd, 1.0);

    // min = (Node *) FH_extract_min(&fh);
    // printf("min: %u %f\n", min - nodes, min->l);
    nd = fh.min_root;
    print_fh(nodes, nd, 0);

    min = (Node *) FH_extract_min(&fh);
    printf("Amin: %u %f\n", min - nodes, min->l);
    nd = fh.min_root;
    print_fh(nodes, nd, 0);

    min = (Node *) FH_extract_min(&fh);
    printf("Amin: %u %f\n", min - nodes, min->l);
    nd = fh.min_root;
    print_fh(nodes, nd, 0);
}
