#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadGrid.h"

/*
    This function returns the grid loaded from a .txt file
 */
gGrid *load_grid(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("An error occurred while opening the file.\n");
        exit(1);
    }

    char line[1024];
    int rows = 0;
    int cols = 0;
    char **grid = NULL;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (cols == 0) {
            cols = strlen(line);
        }

        grid = realloc(grid, sizeof(char *) * (rows + 1));
        grid[rows] = malloc(sizeof(char) * cols);
        strncpy(grid[rows], line, cols);

        rows++;
    }

    fclose(file);

    gGrid *result = malloc(sizeof(gGrid));
    result->grid = grid;
    result->rows = rows;
    result->cols = cols;

    return result;
}
