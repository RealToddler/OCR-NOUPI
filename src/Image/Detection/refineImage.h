#ifndef REFINEIMAGE_H
#define REFINEIMAGE_H

#include "../image.h"
#include "boxes.h"

int is_in_interval(long x, long y, long val);

// one eraser function
void erase(iImage *img, bBoundingBox boxe);

// one sort function (to find the biggest boxe which is the grid)
// mod 1 = height mod 2 = width mod 3 = surface
bBoundingBox *sort(bBoundingBox *boxes, int mod, int numBoxes);

// one function that compute an histogram of some values of the boxes
int *compute_histogram(bBoundingBox *sortedBoxes, int mod, int numBoxes);

// in order to know the median and average sizes
int compute_median(int *histogram, int size);
double compute_average(bBoundingBox *boxes, int mod, int numBoxes);

double compute_median2(bBoundingBox *boxes, int mod, int numBoxes);

#endif // REFINEIMAGE_H