#include "../image.h"
#include "refineImage.h"

void erase(iImage *img, BoundingBox boxe) {
    for (unsigned int y = boxe.min_y; y < boxe.max_y; y++)
    {
        for (unsigned int x = boxe.min_x; x < boxe.max_x; x++)
        {
            pPixel *pixel = &img->pixels[y][x];
            pixel->r = 0;
            pixel->g = 0;
            pixel->b = 0;
        }
    }
}