#ifndef CANNY_H
#define CANNY_H

#include "../image.h"

void calculate_gradients(iImage *img, float **gradient_magnitude,
                         float **gradient_direction);
void non_max_suppression(iImage *img, float **gradient_magnitude,
                         float **gradient_direction, float **edges);
void dilate(unsigned char **input, unsigned char **output, unsigned int height,
            unsigned int width);
void apply_canny(iImage *img);

#endif // CANNY_H
