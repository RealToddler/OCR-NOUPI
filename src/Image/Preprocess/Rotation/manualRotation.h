#ifndef ROTATION_H
#define ROTATION_H

#include "../../image.h"
#include <SDL2/SDL.h>

void rotation_matrix(double theta_deg, double R[2][2]);
void rotate_point(double x, double y, double center_x, double center_y,
                  double R[2][2], double *new_x, double *new_y);
iImage *rotate_image(iImage *image, double angle_deg);

void compute_new_dimensions(unsigned int width, unsigned int height,
                            double angle_deg, unsigned int *new_width,
                            unsigned int *new_height);

#endif
