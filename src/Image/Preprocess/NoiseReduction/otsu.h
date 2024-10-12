#ifndef OTSU_H
#define OTSU_H

#include "../../image.h"

unsigned int compute_otsu_threshold(unsigned int *histogram,
                                    unsigned int total_pixels);
void otsu_threshold(iImage *img, const unsigned int block_size);

#endif