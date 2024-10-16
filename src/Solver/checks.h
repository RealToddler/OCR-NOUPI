#ifndef CHECKS_H
#define CHECKS_H

#define MAX_COLS 1024

typedef struct {
    int x;
    int y;
} tTuple;

typedef struct {
    tTuple t1;
    tTuple t2;
} cCoords;

cCoords check_horizontal(int cols, char **grid, char *word, tTuple first_coords,
                         int word_size);
cCoords check_vertical(int rows, char **grid, char *word, tTuple first_coords,
                       int word_size);
cCoords check_diags(int rows, int cols, char **grid, char *word,
                    tTuple first_coords, int word_size);

#endif
