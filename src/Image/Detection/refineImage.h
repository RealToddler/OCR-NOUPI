#ifndef REFINEIMAGE_H
#define REFINEIMAGE_H

#include "../image.h"
#include "boxes.h"

int is_in_interval(long x, long y, long val);

// one eraser function
void erase(iImage *img, BoundingBox boxe);

// one sort function (to find the biggest boxe which is the grid)
// mod 1 = height mod 2 = width mod 3 = surface
BoundingBox* sort(BoundingBox *boxes, int mod, int numBoxes);

// one function that compute an histogram of some values of the boxes
unsigned int *compute_histogram(BoundingBox *sortedBoxes, int mod, int numBoxes);

// in order to know the median and average sizes
// mod 1 = height mod 2 = width mod 3 = surface
double compute_median(BoundingBox *boxes, int mod);
double compute_average(BoundingBox *boxes, int mod);

#endif // REFINEIMAGE_H