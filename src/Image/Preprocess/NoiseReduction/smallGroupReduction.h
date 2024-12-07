#ifndef SMALLGROUPREDUCTION_H
#define SMALLGROUPREDUCTION_H

#include "Image/image.h"
#include <SDL2/SDL.h>

void apply_groups_reduction(iImage *image);
void clear(iImage *image, int x, int y);
int len_group(iImage *image, int **visited, int y, int x);

#endif // SMALLGROUPREDUCTION_H
