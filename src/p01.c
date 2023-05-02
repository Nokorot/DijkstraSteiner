#include <stdlib.h>
#include <string.h>

#define FLAG_IMPLEMENTATION
#include "flag.h"

typedef char uint8;

#include "parse.h"
#include "graphs.h"
#include "algorithm.h"

void usage(FILE *sink, const char *program)
{
    fprintf(sink, "Usage: %s [-gc] <DATA_FILE>\n", program);
    fprintf(sink, "OPTIONS:\n");
    flag_print_options(sink);
}

int main(int argc, char **argv) {
    const char *program = *argv;
    
    bool *help = flag_bool("h", false, "Print this help to stdout and exit with 0");
    bool *graph = flag_bool("g", false, "Output The Minimal Stiner Tree found by the algorithm");
    bool *counter = flag_bool("c", false, "Counter, prints to the output, per 1k loops, to give a visual indication that something is happening");
    
    if (!flag_parse(argc, argv)) {
        usage(stderr, program);
        flag_print_error(stderr);
        exit(1);
    }
    argv = flag_rest_argv();
    
    if (*help) {
        usage(stdout, program);
        exit(0);
    }

    if (!*argv) {
        printf("Not enough arguments, pleas provide a datafile. (see -h for more info)\n");
        exit(0);
    }


    struct InputData data = parse_file(argv[0]);
    int n = data.n;


    //  The construct_hanan_grid function sorts the xs and ys.
    int *sorted_xs = malloc(sizeof(int)*n);
    memcpy(sorted_xs, data.xs, sizeof(int)*n);

    int *sorted_ys = malloc(sizeof(int)*n);
    memcpy(sorted_ys, data.ys, sizeof(int)*n);

    int yn = data.n, xn = data.n;
    Graph G = constrct_hanan_grid(&xn, &yn, sorted_xs, sorted_ys);

    int *T = malloc(sizeof(Point) * n);
    // Note: This is dependent on the order of the points in G.V.
    
    for (int i=0; i<n; ++i) {
        int j=0, k=0;
        while(sorted_xs[j] < data.xs[i]) ++j;
        while(sorted_ys[k] < data.ys[i]) ++k;

        T[i] = j + xn*k;
    }

    free(sorted_xs);
    free(sorted_ys);

    int result = -1;
    if (*graph) { // Constuct the Minimal Steiner Tree graph.
        Graph mst;
        result = Algorithm((uint8) n, G, T, n-1, &mst, *counter);

        Point *a = mst.V, *b;
        for (int i=0; i<mst.size; ++i, ++a)
            for (int j=0; j<4 && a->neigbours[j]; ++j) {
                b = a->neigbours[j];
                printf("(%u,%u) -- (%u,%u)\n", a->x, a->y, b->x, b->y);
            }

    } else {
        result = Algorithm((uint8) n, G, T, n-1, NULL, *counter);
    }
    printf("Restult: %u\n", result);

    free(data.xs);
    free(data.ys);
    free(T);
    return 0;
}
