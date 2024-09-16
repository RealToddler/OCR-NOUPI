#include "Preprocess/grayscale.h"
#include "Utils/image.h"

void grayscale(iImage *img)
{
    const unsigned int width = img->width;
    const unsigned int height = img->height;

    pPixel pixel;
    double graylevel;
    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            pixel = img->pixels[x][y];
            graylevel = 0.3 * pixel.r + 0.59 * pixel.g + 0.11 * pixel.b;
            img->pixels[x][y].r = graylevel;
            img->pixels[x][y].g = graylevel;
            img->pixels[x][y].b = graylevel;
        }
    }
}