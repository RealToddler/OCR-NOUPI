#include "checks.h"
#include <ctype.h>
#include <stdio.h>

/*
    This function checks horizontally if the word is around.
*/
cCoords check_horizontal(int cols, char **grid, char *word, tTuple first_coords,
                         int word_size) {
    cCoords result;
    result.t1 = first_coords;
    result.t2.x = -1;
    result.t2.y = -1;

    // left
    if (first_coords.x - (word_size - 1) >= 0) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int x = first_coords.x - offset;
            if (grid[first_coords.y][x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x - (word_size - 1);
            result.t2.y = first_coords.y;
            return result;
        }
    }

    // right
    if (first_coords.x + (word_size - 1) < cols) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int x = first_coords.x + offset;
            if (grid[first_coords.y][x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x + (word_size - 1);
            result.t2.y = first_coords.y;
            return result;
        }
    }

    return result;
}

/*
    This function checks vertically if the word is around.
*/
cCoords check_vertical(int rows, char **grid, char *word, tTuple first_coords,
                       int word_size) {
    cCoords result;
    result.t1 = first_coords;
    result.t2.x = -1;
    result.t2.y = -1;

    // up
    if (first_coords.y - (word_size - 1) >= 0) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int y = first_coords.y - offset;
            if (grid[y][first_coords.x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x;
            result.t2.y = first_coords.y - (word_size - 1);
            return result;
        }
    }

    // down
    if (first_coords.y + (word_size - 1) < rows) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int y = first_coords.y + offset;
            if (grid[y][first_coords.x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x;
            result.t2.y = first_coords.y + (word_size - 1);
            return result;
        }
    }

    return result;
}

/*
    This function checks diagonally if the word is around.
*/
cCoords check_diags(int rows, int cols, char **grid, char *word,
                    tTuple first_coords, int word_size) {
    cCoords result;
    result.t1 = first_coords;
    result.t2.x = -1;
    result.t2.y = -1;

    // top left diag
    if (first_coords.y - (word_size - 1) >= 0 &&
        first_coords.x - (word_size - 1) >= 0) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int y = first_coords.y - offset;
            int x = first_coords.x - offset;
            if (grid[y][x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x - (word_size - 1);
            result.t2.y = first_coords.y - (word_size - 1);
            return result;
        }
    }

    // top right diag
    if (first_coords.y - (word_size - 1) >= 0 &&
        first_coords.x + (word_size - 1) < cols) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int y = first_coords.y - offset;
            int x = first_coords.x + offset;
            if (grid[y][x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x + (word_size - 1);
            result.t2.y = first_coords.y - (word_size - 1);
            return result;
        }
    }

    // bottom left diag
    if (first_coords.y + (word_size - 1) < rows &&
        first_coords.x - (word_size - 1) >= 0) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int y = first_coords.y + offset;
            int x = first_coords.x - offset;
            if (grid[y][x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x - (word_size - 1);
            result.t2.y = first_coords.y + (word_size - 1);
            return result;
        }
    }

    // bottom right diag
    if (first_coords.y + (word_size - 1) < rows &&
        first_coords.x + (word_size - 1) < cols) {
        int match = 1;
        for (int offset = 0; offset < word_size; offset++) {
            int y = first_coords.y + offset;
            int x = first_coords.x + offset;
            if (grid[y][x] != word[offset]) {
                match = 0;
                break;
            }
        }
        if (match) {
            result.t2.x = first_coords.x + (word_size - 1);
            result.t2.y = first_coords.y + (word_size - 1);
            return result;
        }
    }

    return result;
}
