#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "graphs.h"
#include <stdbool.h>

typedef char uint8;

// The algorithm allocates a nodes array of the following size, 
// where n = #Terminals and m=|V(G)|.
int sizeofNodes(int n, int m);

int Algorithm(uint8 n, Graph G, int *T, uint8 t, Graph *mst);

#endif
