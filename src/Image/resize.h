#ifndef RESIZE_H
#define RESIZE_H

#include "Image/image.h"

iImage *resize_image(iImage *img, unsigned int new_width,
                     unsigned int new_height);
iImage *resize_image_with_white(iImage *img, unsigned int new_width,
                                unsigned int new_height);

#endif // RESIZE_H