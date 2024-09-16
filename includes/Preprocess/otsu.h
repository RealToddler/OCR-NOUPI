#ifndef OTSU_H
#define OTSU_H

#include "Utils/image.h"

unsigned int compute_otsu_threshold(unsigned int *histogram, unsigned int total_pixels);
void otsu_threshold(iImage *img);

#endif