#include "invertColors.h"
#include "Image/image.h"

/*
    This function inverts the color of an image assuming its already binarized.
*/
void invert_colors(iImage *img) {
    const unsigned int width = img->width;
    const unsigned int height = img->height;

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            pPixel *pixel = &img->pixels[y][x];

            // we assume pixels are only white or black
            if (pixel->r == 0 && pixel->g == 0 && pixel->b == 0) {
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
