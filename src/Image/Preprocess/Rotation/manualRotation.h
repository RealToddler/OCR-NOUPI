#ifndef ROTATION_H
#define ROTATION_H

#include <SDL2/SDL.h>
#include "../../image.h"

void rotation_matrix(double theta_deg, double R[2][2]);
void rotate_point(double x, double y, double center_x, double center_y, double R[2][2], double *new_x, double *new_y);
iImage *rotate_image(iImage *image, double angle_deg);

#endif
