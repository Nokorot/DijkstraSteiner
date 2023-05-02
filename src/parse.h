#include <stdio.h>
#include <stdlib.h>

struct InputData {
    int n;
    int *xs, *ys;
};

int parse_int(char **str);


struct InputData parse_file(char *file) {
    struct InputData data;


    FILE *fd = fopen(file, "r");

    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, fd);
    
    data.n = parse_int(&line);
    if (data.n < 0) {
       fprintf(stderr, "ERROR: Expects n to be non-negarive, got %d!\n", data.n);
       exit(1);
    }

    data.xs = malloc(sizeof(int)*data.n);
    data.ys = malloc(sizeof(int)*data.n);

    int i=0;
    while ((read = getline(&line, &len, fd)) != -1) {
        data.xs[i] = parse_int(&line);
        data.ys[i] = parse_int(&line);

        ++i;
    }

    return data;
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
        fprintf(stderr, "ERROR: Failed to parse string as interger!\n");
        exit(2);
    }
        
    while (++*str) {
        if ('9' < **str || '0' > **str)
            return u;
        u = u*10 + **str - '0';
    }
    return u;
}



