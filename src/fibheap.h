#include <math.h>
#include <stdbool.h>

typedef struct FH_Node {
    void *data;
    // NOTE: Idealy the key sould be stored here, and avoid using the function pointers stored in FibHeap
    // float key

    struct FH_Node *left, *right; // Double linked list
                  
    bool marked;

    int children; // NOTE: I have not tested if this is computed correctly
    int degree; // # Direct Children
    struct FH_Node *min_child; // NOTE: this name is very miss leading since it is not actually the minimum.

    struct FH_Node *parent;
} FH_Node;


typedef struct FibHeap {
    void (*set_key)(void *, float); // This assumes the key is a float
    float (*compare)(void *, void *);

    struct FH_Node *min_root;
    int nr_roots; 
    int size;  // NOTE: I have not tested if this is computed correctly

    FH_Node **roots; // Array of length log(n)
} FibHeap;

// void FH_init(int n);  Sould be implemented

void FH_merge(FibHeap *a, FibHeap *b);
void FH_insert(FibHeap *a, FH_Node *nd);
void *FH_extract_min(FibHeap *a);
void FH_decrease_key(FibHeap *a, FH_Node *nd, float new_key);
void FH_del(FibHeap *a, FH_Node *nd);

