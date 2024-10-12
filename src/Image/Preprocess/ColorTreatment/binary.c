#include "binary.h"
#include "../Image/image.h"

void binary(iImage *img) {
    unsigned int width = img->width;
    unsigned int height = img->height;

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            pPixel *pixel = &img->pixels[y][x];

            unsigned int avg = (pixel->r + pixel->g + pixel->b) / 3;

            if (avg >= 127) {
                pixel->r = 255;
                pixel->g = 255;
                pixel->b = 255;
            } else {
                pixel->r = 0;
                pixel->g = 0;
                pixel->b = 0;
            }
        }
    }
}