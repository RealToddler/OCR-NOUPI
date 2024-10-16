#include "../Image/image.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/*
    This function applies a Gaussian Blur to a given image
*/
void apply_gaussian_blur(iImage *image) {
    const float gaussian_kernel[3][3] = {{1 / 16.0, 1 / 8.0, 1 / 16.0},
                                         {1 / 8.0, 1 / 4.0, 1 / 8.0},
                                         {1 / 16.0, 1 / 8.0, 1 / 16.0}};

    int width = image->width;
    int height = image->height;

    pPixel **blurred_pixels = (pPixel **)malloc(height * sizeof(pPixel *));
    for (int i = 0; i < height; i++) {
        blurred_pixels[i] = (pPixel *)malloc(width * sizeof(pPixel));
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float r = 0.0, g = 0.0, b = 0.0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int nx = x + i;
                    int ny = y + j;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        pPixel *current_pixel = &image->pixels[ny][nx];
                        float weight = gaussian_kernel[i + 1][j + 1];

                        r += current_pixel->r * weight;
                        g += current_pixel->g * weight;
                        b += current_pixel->b * weight;
                    }
                }
            }

            blurred_pixels[y][x].r = (Uint8)r;
            blurred_pixels[y][x].g = (Uint8)g;
            blurred_pixels[y][x].b = (Uint8)b;
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image->pixels[y][x] = blurred_pixels[y][x];
        }
    }

    for (int i = 0; i < height; i++) {
        free(blurred_pixels[i]);
    }
    free(blurred_pixels);
}
