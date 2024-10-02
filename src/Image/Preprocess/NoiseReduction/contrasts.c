#include "../Image/image.h"

void increase_contrast(iImage *img, float alpha, float threshold)
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
    float mean = sum / (width * height); // Calcul de la luminance moyenne

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            pPixel *pixel = &img->pixels[y][x];
            float pixel_value = pixel->r; // Utilisation du canal rouge comme intensité de référence

            if (fabs(pixel_value - mean) > threshold)
            {
                int new_intensity = (int)(alpha * (pixel_value - mean) + mean);

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
}
