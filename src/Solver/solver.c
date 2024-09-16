#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checks.h"

void printGrid(char grid[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

int main() {
    const char *word = "CACA";
    int rows = 10;
    int cols = 10;
    int word_size = (int)strlen(word);

    int x =0;
    int y =0;

    char grid[10][10] = {
        {'A', 'C', 'B', 'D', 'K', 'F', 'R', 'H', 'I', 'J'},
        {'K', 'E', 'M', 'E', 'C', 'U', 'K', 'R', 'S', 'T'},
        {'U', 'V', 'O', 'T', 'O', 'O', 'A', 'K', 'C', 'D'},
        {'E', 'F', 'G', 'E', 'N', 'J', 'K', 'L', 'M', 'N'},
        {'O', 'K', 'N', 'J', 'J', 'O', 'N', 'R', 'J', 'X'}, // here
        {'Y', 'O', 'O', 'V', 'U', 'O', 'E', 'O', 'G', 'H'},
        {'B', 'U', 'K', 'L', 'C', 'A', 'C', 'A', 'B', 'R'},
        {'R', 'T', 'U', 'V', 'W', 'X', 'Y', 'R', 'A', 'K'},
        {'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'},
        {'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'}
    };    

    tTuple first_coords;
        
    while (1)
    {
        printf("x = %d      y = %d\n", x, y);
        first_coords = find_first_letter(x, y, rows, cols, grid, word);

        if (first_coords.x == -69 && first_coords.y == -69 )
        {
            printf("Not found.\n");
            break;
        }

        cCoords result = check_horizontal(rows, cols, grid, word, first_coords, word_size);
        if (result.t2.x != -69 && result.t2.y != -69)
        {
            printf("(%d, %d) à (%d, %d)\n", result.t1.x, result.t1.y, result.t2.x, result.t2.y);
            break;
        }

        result = check_vertical(rows, cols, grid, word, first_coords, word_size);
        if (result.t2.x != -69 && result.t2.y != -69)
        {
            printf("(%d, %d) à (%d, %d)\n", result.t1.x, result.t1.y, result.t2.x, result.t2.y);
            break;
        }

        result = check_diags(rows, cols, grid, word, first_coords, word_size);
        if (result.t2.x != -69 && result.t2.y != -69)
        {
            printf("(%d, %d) à (%d, %d)\n", result.t1.x, result.t1.y, result.t2.x, result.t2.y);
            break;
        }

        x = first_coords.x;
        y = first_coords.y;

        // grid[first_coords.y][first_coords.x] = '-';
    }
        return 0;
    }
