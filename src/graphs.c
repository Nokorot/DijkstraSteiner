#include <stdlib.h>
#include <string.h>

#include "graphs.h"

int compare(const void *a, const void *b) {
  return ( *(int*)a - *(int*)b );
}

int *undupe(int *xs) {
    int *x = xs;
    while (*(++xs)) {
        if (*x != *xs) 
            ++x;
        *x = *xs;
    }
    return x;
}

Graph constrct_hanan_grid(int *xn, int *yn, int *xs, int *ys) {
    qsort (xs, *xn, sizeof(int), compare);
    *xn = undupe(xs) - xs+ 1;

    qsort (ys, *yn, sizeof(int), compare);
    *yn = undupe(ys) - ys + 1;
    
    int m = (*xn)*(*yn);
    Point *V = malloc(sizeof(Point)*m);
    memset(V, 0, sizeof(Point)*m);

    Point *p;
    for (int i=0; i<*xn; ++i)
        for (int j=0; j<*yn; ++j) {
            p = &V[i+*xn*j];
            p->x = xs[i]; p->y = ys[j];
            if (0 < i)      p->neigbours[0] = V + (i-1) + (*xn)*j;
            if (i < *xn-1)  p->neigbours[1] = V + (i+1) + (*xn)*j;
            if (0 < j)      p->neigbours[2] = V + i     + (*xn)*(j-1);
            if (j < *yn-1)  p->neigbours[3] = V + i     + (*xn)*(j+1);
        }

    return (Graph) { V, m };
}
