#ifndef DENOISE_H
#define DENOISE_H

#include <SDL2/SDL.h>
#include "Utils/image.h"

pPixel apply_gaussian_filter(iImage *image, int x, int y);
void apply_unsharp_mask(iImage *image, float sharpness);

#endif 
