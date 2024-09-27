#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "../Image/image.h"

pPixel apply_gaussian_filter(iImage *image, int x, int y)
{
    // Masque gaussien 3x3 (sigma = 1.0)
    float gaussian_kernel[3][3] = {
        {1 / 16.0, 1 / 8.0, 1 / 16.0},
        {1 / 8.0, 1 / 4.0, 1 / 8.0},
        {1 / 16.0, 1 / 8.0, 1 / 16.0}};

    float r = 0.0, g = 0.0, b = 0.0;
    int filter_size = 3;
    int half_size = filter_size / 2;

    for (int i = -half_size; i <= half_size; i++)
    {
        for (int j = -half_size; j <= half_size; j++)
        {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < image->width && ny >= 0 && ny < image->height)
            {
                pPixel *current_pixel = &image->pixels[ny][nx];
                float weight = gaussian_kernel[i + half_size][j + half_size];
                r += current_pixel->r * weight;
                g += current_pixel->g * weight;
                b += current_pixel->b * weight;
            }
        }
    }

    pPixel blurred_pixel;
    blurred_pixel.r = (Uint8)r;
    blurred_pixel.g = (Uint8)g;
    blurred_pixel.b = (Uint8)b;

    return blurred_pixel;
}

void apply_gaussian_blur(iImage *image)
{
    iImage blurred_image;
    blurred_image.height = image->height;
    blurred_image.width = image->width;
    blurred_image.pixels = (pPixel **)malloc(image->height * sizeof(pPixel *));

    // Appliquer le filtre gaussien à chaque pixel
    for (unsigned int i = 0; i < image->height; i++)
    {
        blurred_image.pixels[i] = (pPixel *)malloc(image->width * sizeof(pPixel));
        for (unsigned int j = 0; j < image->width; j++)
        {
            blurred_image.pixels[i][j] = apply_gaussian_filter(image, j, i);
        }
    }

    // Copier les pixels floutés dans l'image originale
    for (unsigned int i = 0; i < image->height; i++)
    {
        for (unsigned int j = 0; j < image->width; j++)
        {
            image->pixels[i][j] = blurred_image.pixels[i][j];
        }
    }

    // Libérer la mémoire de l'image floutée temporaire
    for (unsigned int i = 0; i < blurred_image.height; i++)
    {
        free(blurred_image.pixels[i]);
    }
    free(blurred_image.pixels);
}
