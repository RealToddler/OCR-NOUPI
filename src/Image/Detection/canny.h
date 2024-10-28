#ifndef CANNY_H
#define CANNY_H

#include "../image.h"
#include "boxes.h"

void calculate_gradients(iImage *img, float **gradient_magnitude,
                         float **gradient_direction);
void non_max_suppression(iImage *img, float **gradient_magnitude,
                         float **gradient_direction, float **edges);
void dilate(unsigned char **input, unsigned char **output, int height,
            int width);

void apply_canny(void (*func)(BoundingBox *, int, iImage *), iImage *img);

#endif // CANNY_H
