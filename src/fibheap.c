#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fibheap.h"

void extract_min_phase2( FibHeap *a, FH_Node *nd, int *max_dg);

void FH_merge(FibHeap *a, FibHeap *b) 
{
    if (!b->min_root) 
        return;
    if (!a->min_root) {
        a->min_root = b->min_root;
        a->size = b->size;
        a->nr_roots = b->nr_roots;
        return;
    }

    a->min_root->left->right = b->min_root;
    b->min_root->left->right = a->min_root;

    FH_Node *_nd = b->min_root->left;
    b->min_root->left = a->min_root->left;
    a->min_root->left = _nd;
    
    a->nr_roots = a->nr_roots + b->nr_roots;

    a->size = a->size + b->size;

    // a < b
    if (a->compare(a->min_root->data, b->min_root->data) > 0)
        a->min_root = b->min_root;
}

void FH_insert(FibHeap *a, FH_Node *nd) 
{
    // Assume: memset(nd, 0, sizeof(FH_Node));
    nd->left = nd->right = nd;
    // { min_root: nd, size: nd->children+1, nr_roots: 1 };
    FibHeap b = { 0 };
    b.min_root = nd; 
    b.size = nd->children+1;
    b.nr_roots = 1;
    FH_merge(a,&b);
}

void extract_min_phase2(FibHeap *a, FH_Node *nd, int *max_dg) 
{
    int dg = nd->degree;
    if (dg > *max_dg) *max_dg = dg;
    
    FH_Node **roots = a->roots;

    FH_Node *nd_u, *nd_v;
    if (roots[dg]) {
        // assert(roots[dg] != nd);
        
        float cmp = a->compare(roots[dg]->data, nd->data);
        if (cmp < 0) { // nd < roots[dg]
            nd_u = roots[dg];
            nd_v = nd;
        } else {
            nd_u = nd;
            nd_v = roots[dg];
        }

        // Remove nd_v as a root
        nd_v->left->right = nd_v->right;
        nd_v->right->left = nd_v->left;
        nd_v->marked = 0;
        nd_v->parent = nd_u;

        a->nr_roots--;

        //  Insert nd_v as child of nd_u
        // Note: Here min_child might not be min
        if (nd_u->min_child) {
            nd_u->min_child->left->right = nd_v;
            nd_v->right                  = nd_u->min_child;
            nd_v->left                   = nd_u->min_child->left;
            nd_u->min_child->left        = nd_v;
        } else {
            nd_u->min_child = nd_v;
            nd_v->left = nd_v->right = nd_v;
        }

        // if (nd_v == a->min_root) 
        //     a->min_root = nd_u;

        nd_u->children = nd_v->children + nd_u->children + 1;

        // Degree = # Direct children
        nd_u->degree++;
        // This node has a different degree now. 
        roots[dg] = NULL;

        extract_min_phase2(a, nd_u, max_dg);
    } else {
        roots[dg] = nd;
    }
}

void *FH_extract_min(FibHeap *a) 
{
    // assert(!a->min_root);

    // Phase 1, Extract min_root.
    FH_Node *min = a->min_root;
    a->size--;
    a->nr_roots--;

    // This also includes the case where there is only one node left
    if (a->size == 0) {
        a->min_root = NULL;
        return min->data;
    }

    FH_Node *nd, *_nd;

    if (min->min_child) {
        // ? Marked nodes ?
        // FibHeap b = { min_root: min->min_child, size: min->children };
        // merge(a, &b);

        min->left->right = min->min_child;
        min->min_child->left->right = min->right; // Skip min_root
 
        FH_Node *_nd = min->min_child->left;
        min->min_child->left = min->left;
        min->right->left = _nd;

        a->min_root = min->right;

        a->nr_roots = a->nr_roots + min->degree;

        // NOTE: parent = NULL indicates that it is a root. 
        //  It took me so many hours trying to debug a problem coused 
        //  the lack of this for loop
        _nd = min->min_child;
        for (int j = 0; j < min->degree; ++j, _nd = _nd->right)
            _nd->parent = NULL;
    } else { 
        min->left->right = min->right;
        min->right->left = min->left;
        a->min_root = min->right;
    }

    // Phase 2
    int max_dg = 0;
    // NOTE: by starting to the right, the loop is sciped if there is only one root.
    nd = a->min_root->right;
    while (nd != a->min_root) {
        _nd = nd->right;
        extract_min_phase2(a, nd, &max_dg);
        nd = _nd;
    }

    memset(a->roots, 0, sizeof(FH_Node *) * (max_dg+1));

    
    // Phase 3
    _nd = a->min_root;
    for (nd = a->min_root->right; nd != a->min_root; nd = nd->right) {
        if (a->compare(nd->data, _nd->data) < 0)
            _nd = nd;
    }
    a->min_root = _nd;

    return min->data;
}

void FH_decrease_key(FibHeap *a, FH_Node *nd, float new_key) 
{
    a->set_key(nd->data, new_key);

    // nd->data < nd->parent->data
    if (nd->parent && a->compare(nd->data, nd->parent->data) < 0) { 
        FH_Node *_nd;
        while (nd->parent) {
            if (nd->parent->degree == 1) {
                nd->parent->min_child = NULL;
            } else if (nd->parent->min_child == nd)
                nd->parent->min_child = nd->right;
            nd->parent->children--;
            nd->parent->degree--;

            nd->right->left = nd->left;
            nd->left->right = nd->right;

            _nd = nd->parent;
            FH_insert(a, nd);
            nd->parent = NULL;

            if (!_nd->marked) {
                _nd->marked = true;
                break;
            }
            nd = _nd;
        }

        if (a->compare(nd->data, a->min_root->data) < 0) 
            a->min_root = nd;
    }
}

void FH_del(FibHeap *a, FH_Node *nd)
{
    a->set_key(nd->data, -INFINITY);
}
