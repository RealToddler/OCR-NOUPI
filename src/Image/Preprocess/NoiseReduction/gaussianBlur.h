#ifndef DENOISE_H
#define DENOISE_H

#include "../../image.h"
#include <SDL2/SDL.h>

pPixel apply_gaussian_filter(iImage *image, int x, int y);
void apply_gaussian_blur(iImage *image);

#endif // DENOISE_H
