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

void free_grid(gGrid *grid);

char *get_val(gGrid *coords, int row, int col);

gGrid *load_grid(char *filename);

#endif // LOADGRID_H