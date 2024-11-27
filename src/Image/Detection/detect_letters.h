#ifndef DETECT_LETTERS_H
#define DETECT_LETTERS_H

#include "boxes.h"
#include "../Image/image.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
    bBoundingBox * boxes;
    int size;
} bBoundingBox_size;

void draw_group(iImage *image, int **visited, int y, int x, int *xi, int *yi, int *xf, int *yf);
int is_color2(iImage *image, int y, int x);
bBoundingBox_size apply_groups_box(iImage *image);

#endif 