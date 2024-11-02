#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checks.h"
#include "loadGrid.h"

/*
    This function checks if the word is in the grid.
*/
void solver(char *word, char *filename) {
    gGrid *grid = load_grid(filename);

    tTuple first_coords;
    int word_size = strlen(word);

    for (int i = 0; i < word_size; i++) {
        word[i] = toupper(word[i]);
    }

    int found = 0;

    for (int y = 0; y < grid->rows && !found; y++) {
        for (int x = 0; x < grid->cols && !found; x++) {
            first_coords.x = x;
            first_coords.y = y;

            cCoords result = check_horizontal(grid->cols, grid->grid, word,
                                              first_coords, word_size);
            if (result.t2.x != -1 && result.t2.y != -1) {
                printf("(%d,%d)(%d,%d)\n", result.t1.x, result.t1.y,
                       result.t2.x, result.t2.y);
                found = 1;
                break;
            }

            result = check_vertical(grid->rows, grid->grid, word, first_coords,
                                    word_size);
            if (result.t2.x != -1 && result.t2.y != -1) {
                printf("(%d,%d)(%d,%d)\n", result.t1.x, result.t1.y,
                       result.t2.x, result.t2.y);
                found = 1;
                break;
            }

            result = check_diags(grid->rows, grid->cols, grid->grid, word,
                                 first_coords, word_size);
            if (result.t2.x != -1 && result.t2.y != -1) {
                printf("(%d,%d)(%d,%d)\n", result.t1.x, result.t1.y,
                       result.t2.x, result.t2.y);
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        printf("Not Found\n");
    }

    // free the memory
    for (int i = 0; i < grid->rows; i++) {
        free(grid->grid[i]);
    }
    free(grid->grid);
    free(grid);
}
