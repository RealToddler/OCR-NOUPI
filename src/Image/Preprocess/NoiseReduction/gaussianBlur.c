#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../Image/image.h"

void apply_gaussian_blur(iImage *image)
{
    // Masque gaussien 3x3 (sigma = 1.0) précalculé
    const float gaussian_kernel[3][3] = {
        {1 / 16.0, 1 / 8.0, 1 / 16.0},
        {1 / 8.0, 1 / 4.0, 1 / 8.0},
        {1 / 16.0, 1 / 8.0, 1 / 16.0}};

    int width = image->width;
    int height = image->height;

    // Crée une nouvelle image pour stocker le résultat
    pPixel **blurred_pixels = (pPixel **)malloc(height * sizeof(pPixel *));
    for (int i = 0; i < height; i++)
    {
        blurred_pixels[i] = (pPixel *)malloc(width * sizeof(pPixel));
    }

    // Applique le filtre gaussien
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float r = 0.0, g = 0.0, b = 0.0;

            // Applique le noyau de convolution
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    int nx = x + i;
                    int ny = y + j;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                    {
                        pPixel *current_pixel = &image->pixels[ny][nx];
                        float weight = gaussian_kernel[i + 1][j + 1];

                        r += current_pixel->r * weight;
                        g += current_pixel->g * weight;
                        b += current_pixel->b * weight;
                    }
                }
            }

            // Assigne les nouvelles valeurs au pixel
            blurred_pixels[y][x].r = (Uint8)r;
            blurred_pixels[y][x].g = (Uint8)g;
            blurred_pixels[y][x].b = (Uint8)b;
        }
    }

    // Copie les pixels floutés dans l'image d'origine
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image->pixels[y][x] = blurred_pixels[y][x];
        }
    }

    // Libère la mémoire allouée
    for (int i = 0; i < height; i++)
    {
        free(blurred_pixels[i]);
    }
    free(blurred_pixels);
}
