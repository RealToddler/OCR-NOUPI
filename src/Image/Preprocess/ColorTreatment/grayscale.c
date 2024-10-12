#include "grayscale.h"
#include "../Image/image.h"

void grayscale(iImage *img) {
    const unsigned int width = img->width;
    const unsigned int height = img->height;

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            pPixel *pixel = &img->pixels[y][x];
            double graylevel =
                0.3 * pixel->r + 0.59 * pixel->g + 0.11 * pixel->b;
            unsigned char gray = (unsigned char)graylevel;

            pixel->r = gray;
            pixel->g = gray;
            pixel->b = gray;
        }
    }
}
