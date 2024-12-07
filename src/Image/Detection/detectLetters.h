#ifndef DETECTLETTERS_H
#define DETECTLETTERS_H

#include "Image/image.h"
#include "boxes.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
  bBoundingBox *boxes;
  int size;
} bBoundingBox_size;

void draw_group(iImage *image, int **visited, int y, int x, int *xi, int *yi,
                int *xf, int *yf);
int is_color2(iImage *image, int y, int x);
bBoundingBox_size apply_groups_box(iImage *image);

#endif // DETECTLETTERS_H