#include "Preprocess/binary.h"
#include "Utils/image.h"

void binary(iImage *img)
{
    int width = img->width;
    int height = img->height;

    // For each pixel of the img
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pPixel pixel = image->pixels[x][y];

            if ((pixel.r + pixel.b + pixel.g) / 3 >= 127)
            {
                img->pixels[x][y].r = 255;
                img->pixels[x][y].b = 255;
                img->pixels[x][y].g = 255;
            }
            else
            {
                img->pixels[x][y].r = 0;
                img->pixels[x][y].b = 0;
                img->pixels[x][y].g = 0;
            }
        }
    }
}
