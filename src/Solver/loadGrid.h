#ifndef LOADGRID_H
#define LOADGRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **grid;
    int rows;
    int cols;
} gGrid;

gGrid *load_grid(char *filename);

#endif // LOADGRID_H