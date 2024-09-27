#include "../Image/image.h"

void increase_contrast(iImage *img, float alpha)
{
    unsigned int width = img->width;
    unsigned int height = img->height;

    float sum = 0;
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            pPixel *pixel = &img->pixels[y][x];
            sum += pixel->r;
        }
    }
    float mean = sum / (width * height);

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            pPixel *pixel = &img->pixels[y][x];
            int new_intensity = (int)(alpha * (pixel->r - mean) + mean);

            if (new_intensity > 255)
                new_intensity = 255;
            else if (new_intensity < 0)
                new_intensity = 0;

            pixel->r = (uint8_t)new_intensity;
            pixel->g = (uint8_t)new_intensity;
            pixel->b = (uint8_t)new_intensity;
        }
    }
}