#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checks.h"
#include "loadGrid.h"

void solver(char *word, char *filename) {
    gGrid *grid = load_grid(filename);

    tTuple first_coords;
    int x = 0;
    int y = 0;
    int word_size = strlen(word);

    while (1) {
        printf("x = %d      y = %d\n", x, y);
        first_coords =
            find_first_letter(x, y, grid->rows, grid->cols, grid->grid, word);

        if (first_coords.x == -69 && first_coords.y == -69) {
            printf("Not found.\n");
            break;
        }

        cCoords result = check_horizontal(grid->cols, grid->grid, word,
                                          first_coords, word_size);
        if (result.t2.x != -69 && result.t2.y != -69) {
            printf("(%d, %d) to (%d, %d)\n", result.t1.x, result.t1.y,
                   result.t2.x, result.t2.y);
            break;
        }

        result = check_vertical(grid->rows, grid->grid, word, first_coords,
                                word_size);
        if (result.t2.x != -69 && result.t2.y != -69) {
            printf("(%d, %d) to (%d, %d)\n", result.t1.x, result.t1.y,
                   result.t2.x, result.t2.y);
            break;
        }

        result = check_diags(grid->rows, grid->cols, grid->grid, word,
                             first_coords, word_size);
        if (result.t2.x != -69 && result.t2.y != -69) {
            printf("(%d, %d) to (%d, %d)\n", result.t1.x, result.t1.y,
                   result.t2.x, result.t2.y);
            break;
        }

        x = first_coords.x;
        y = first_coords.y;
    }
    free(grid);
}