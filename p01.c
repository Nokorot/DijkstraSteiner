#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef char uint8;


typedef struct Node {
    bool defined; // This is set to True when the note is initialized.

    uint8 v;
    uint8 *I; // char[n]. Note: Could do bit map.

    int l;

    // bool inP;

    struct Node *next;
    struct Node *prev;
} Node;

typedef struct SLList {
    struct Node *first;
    int size;
} SLList;

// This does not work. Do not use!
struct Node *Node_new(struct SLList *sll, uint8 v, uint8 *I, uint8 n) {
    struct Node *nd = malloc(sizeof(Node) + sizeof(uint8) * n);
    nd->defined = 1;
    nd->v = v;
    nd->I = (uint8 *) (nd++); //  This syntax might be confusng
    memcpy(nd->I, I, sizeof(uint8) * n);

    nd->next = NULL; // This could be done later
    return nd;
}

void SLL_del(struct SLList *sll) {
    struct Node *nd = sll->first;
    while (nd) 
        free(nd);
}

void SLL_add(struct SLList *sll, Node *nd) {

    if (!sll->first) {
        nd->prev = NULL; // This shuld not be nessesary
        nd->next = NULL; // This shuld not be nessesary
        sll->first = nd;
        return;
    }
    if (nd->l <= sll->first->l) {
        nd->prev = NULL; // This shuld not be nessesary
        nd->next = sll->first;
        nd->next->prev = nd;
        sll->first = nd;
        return;
    }
    
    // IDEA: Could stor the cursor in SLL
    Node *cursor = sll->first;
    while (cursor->next && nd->l <= cursor->next->l)
        cursor = cursor->next;
    
    nd->prev = cursor;
    nd->next = cursor->next;
    if (nd->next)
        nd->next->prev = nd;
    cursor->next = nd;
    return;
}

void SLL_drop(struct SLList *sll, Node *nd) {
   if (sll->first == nd) {
       sll->first = NULL;
       return;
   }

   if (nd->prev) 
       nd->prev->next = nd->next;
   if (nd->next)
       nd->next->prev = nd->prev;
   nd->prev = NULL;
   nd->next = NULL;
}


void SLL_update(SLList *sll, Node *nd, int l) {
    
    // Note that we only realy cear about makeing l smaller
}

// struct {
//    uint8 v;
//    uint8 I;
//    int idx;
// }
// TODO: replace (v,I) with idx everywhere

void def_l(SLList *notP, Node *l_fn, uint8 *Is,   uint8 n, uint8 v, uint8 *I, int l) {
    int idx = v;
    printf("B: %u\n", v);
    for (int j=0; j<n; ++j)  {
        printf("B: %u %u\n", j, I[j]);
        idx = (idx << 1) + I[j];
    }

    Node *nd = &l_fn[idx];
    printf("B: %u\n", idx);
    if (!nd->defined) {
        nd->defined = true;
    
        nd->v = v;
        nd->I = &Is[idx * n]; //  This syntax might be confusng
        memcpy(nd->I, I, sizeof(uint8) * n);

        nd->l = l;
        nd->prev = NULL;
        nd->next = NULL;

        SLL_add(notP, nd);
    } else if (nd->l != l && nd->prev) {
        SLL_drop(notP, nd);
        SLL_add(notP, nd);
    } else {
        return;
    }
}

int algorithm(uint8 n, int *xs, int *ys, uint8 t) {
    printf("n: %u, sizeof(l): %u\n", n, sizeof(Node) * (2 << n));

    struct Node *l_fn = malloc(sizeof(Node) * (2 << n));
    memset(l_fn, 0, sizeof(Node) * (2 << n)); 
    // Could almost just set all the values to l, INTMAX
                           // but this is licly faster
                           
    uint8 *Is = malloc(sizeof(uint8) * (2 << n));

    uint8 *I = malloc(sizeof(uint8) * n); // Shuld be on stack
    SLList notP = { first: NULL, size: 0, };

    // Stage 1:
    memset(I, 0, n);
    
    for (uint8 j=0; j<n; ++j)
        if (j != t) {
            I[j] = 1;
            printf("A\n");
            def_l(&notP, l_fn, Is,  n, j, I, 0);
            I[j] = 0;
        }

    printf("%u\n", notP.first->next->l);


    // int *P = malloc(sizeof(int) * (2 << n));
    // P sould be a sorted linked list.
    //
    //
    
    // SLL_del(&notP);
    free(l_fn);
    free(Is);
    free(I);
    return -1;
}

int parse_int(char **str) {
    int u;

    // scip spaces
    while (**str == ' ') ++*str;

    if (**str == '-')
        u = '0' - *(++*str);
    else
        u = **str - '0';
   
    if ('9' < **str || '0' > **str) {
        fprintf(stderr, "ERROR: Failed to parse string as interger!\n", **str);
        exit(2);
    }
        
    while (++*str) {
        if ('9' > **str || '0' < **str)
            return u;
        u = u*10 + **str - '0';
    }
    return u;
}

int main(int argc, char **argv) {
    
    FILE *fd = fopen(argv[1], "r");

    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, fd);
    
    int n = parse_int(&line);
    if (n < 0) {
       fprintf(stderr, "ERROR: Expects n to be non-negarive, got %d!\n", n);
       exit(1);
    }

    int *xs = malloc(sizeof(int)*n);
    int *ys = malloc(sizeof(int)*n);

    int i=0;
    while ((read = getline(&line, &len, fd)) != -1) {
        xs[i] = parse_int(&line);
        ys[i] = parse_int(&line);

        printf("%u: %u %u \n", i, xs[i], ys[i]);
        ++i;
    }

    algorithm((uint8) n, xs, ys, 0);
}

