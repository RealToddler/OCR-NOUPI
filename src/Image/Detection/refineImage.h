#ifndef REFINEIMAGE_H
#define REFINEIMAGE_H

#include "../image.h"
#include "boxes.h"

// one eraser function
void * erase(iImage img, BoundingBox boxe);

// one sort function (to find the biggest boxe which is the grid)
// mod 1 = height mod 2 = width mod 3 = surface
BoundingBox* sort(BoundingBox *boxes, int mod);

// one function that compute an histogram of some values of the boxes
// mod 1 = height mod 2 = width mod 3 = surface
double* compute_histogram(BoudingBox *boxes, int mod);

// in order to know the median and average sizes
// mod 1 = height mod 2 = width mod 3 = surface
double compute_median(BoundingBox *boxes, mod);
double compute_average(BoudingBox *boxes, mod);

#endif // REFINEIMAGE_H