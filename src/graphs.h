#ifndef GRAPHS_H 
#define GRAPHS_H 

#define MAX_NEIGBOUR_COUNT 4

typedef struct Point {
    int x, y;
    // struct Edge *delta; // Adjestent edges
    struct Point *neigbours[MAX_NEIGBOUR_COUNT];
} Point;

// typedef struct Edge {
//     struct Point *a, *b;
//     int cost;
// } Edge;

typedef struct Graph {
    struct Point *V; // Vertecies
    // struct Edge *E; // Edges
    int size;
} Graph;

Graph constrct_hanan_grid(int *xn, int *yn, int *xs, int *ys);

#endif
