#ifndef SAUVOLA_H
#define SAUVOLA_H

#include "../../image.h"

void sauvola_threshold(iImage *img, const unsigned int block_size);
void dilate_image(iImage *img, const unsigned int kernel_size);

#endif // SAUVOLA_H