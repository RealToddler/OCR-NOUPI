#ifndef SAUVOLA_H
#define SAUVOLA_H

#include "Image/image.h"

void sauvola_threshold(iImage *img, const unsigned int block_size);
void sauvola_threshold_adaptative(iImage *img, const unsigned int block_size);

#endif // SAUVOLA_H