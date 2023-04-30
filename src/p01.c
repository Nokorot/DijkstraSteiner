#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char uint8;

#include "graphs.h"
#include "algorithm.h"


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
        if ('9' < **str || '0' > **str)
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

        ++i;
    }
    
    // Need to sort xs and ys

    int *new_xs = malloc(sizeof(int)*n);
    memcpy(new_xs, xs, sizeof(int)*n);
    int xn = n;

    int *new_ys = malloc(sizeof(int)*n);
    memcpy(new_ys, ys, sizeof(int)*n);
    int yn = n;

    Graph G = constrct_hanan_grid(&xn, &yn, new_xs, new_ys);

    int *T = malloc(sizeof(Point) * n);
    // Note: This is dependent on the order of the points in G.V.
    for (int i=0; i<n; ++i) {
        int j=0, k=0;
        while(new_xs[j] < xs[i]) ++j;
        while(new_ys[k] < ys[i]) ++k;

        // T[i] = G.V[j+xn*k];
        T[i] = j + xn*k;

        printf("T: %u %u\n", G.V[T[i]].x, G.V[T[i]].y);
    }

    Graph mst;

    if (false) { // Constuct the Minimal Steiner Tree graph.
        int result = Algorithm((uint8) n, G, T, 0, &mst);
        printf("Restult: %u\n", result);

        Point *a = mst.V, *b;
        for (int i=0; i<mst.size; ++i, ++a) {
            for (int j=0; j<4 && a->neigbours[j]; ++j) {
                b = a->neigbours[j];
                printf("(%u,%u) -- (%u,%u)\n", a->x, a->y, b->x, b->y);
            }
        }
    } else {
        int result = Algorithm((uint8) n, G, T, 0, NULL);
        printf("Restult: %u\n", result);
    }

    return 0;
}
