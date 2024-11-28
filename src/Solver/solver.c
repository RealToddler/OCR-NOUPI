#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadGrid.h"
#include "checks.h"

cCoords solver(char *word, gGrid *grid) {
    tTuple first_coords;
    int word_size = strlen(word);

    for (int i = 0; i < word_size; i++) {
        word[i] = toupper((unsigned char)word[i]);
    }

    for (int y = 0; y < grid->rows; y++) {
        for (int x = 0; x < grid->cols; x++) {
            first_coords.x = x;
            first_coords.y = y;

            cCoords result;

            result = check_horizontal(grid->cols, grid->grid, word,
                                      first_coords, word_size);
            if (result.t2.x != -1 && result.t2.y != -1) {
                return result;
            }

            result = check_vertical(grid->rows, grid->grid, word, first_coords,
                                    word_size);
            if (result.t2.x != -1 && result.t2.y != -1) {
                return result;
            }

            result = check_diags(grid->rows, grid->cols, grid->grid, word,
                                 first_coords, word_size);
            if (result.t2.x != -1 && result.t2.y != -1) {
                return result;
            }
        }
    }

    return (cCoords){(tTuple){-1, -1}, (tTuple){-1, -1}};
}
