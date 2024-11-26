#include "sauvola.h"
#include "Image/image.h"
#include <math.h>

void sauvola_threshold(iImage *img, const unsigned int block_size) {
    unsigned int width = img->width;
    unsigned int height = img->height;
    double k = 0.5;
    double R = 128.0;

    for (unsigned int by = 0; by < height; by += block_size) {
        for (unsigned int bx = 0; bx < width; bx += block_size) {
            unsigned int sum = 0;
            unsigned int sum_sq = 0;
            unsigned int count = 0;

            for (unsigned int y = by; y < by + block_size && y < height; y++) {
                for (unsigned int x = bx; x < bx + block_size && x < width;
                     x++) {
                    pPixel *pixel = &img->pixels[y][x];
                    unsigned int intensity = pixel->r;
                    sum += intensity;
                    sum_sq += intensity * intensity;
                    count++;
                }
            }

            double mean = sum / (double)count;
            double variance =
                (sum_sq - (sum * sum) / (double)count) / (double)count;
            if (variance < 0)
                variance = 0;
            double std_dev = sqrt(variance);

            double threshold = mean * (1 + k * ((std_dev / R) - 1));

            for (unsigned int y = by; y < by + block_size && y < height; y++) {
                for (unsigned int x = bx; x < bx + block_size && x < width;
                     x++) {
                    pPixel *pixel = &img->pixels[y][x];
                    if (pixel->r > threshold) {
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
    }
}

void dilate_image(iImage *img, const unsigned int kernel_size) {
    int width = img->width;
    int height = img->height;

    iImage *output_img = create_image(width, height, "ras.png");

    int offset = kernel_size / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dilate = 0;
            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    if (y + ky < 0 || x + kx < 0) continue;
                    unsigned int ny = y + ky;
                    unsigned int nx = x + kx;
                    // Vérification si le voisin est dans les limites de l'image
                    if (ny < height && nx < width) {
                        if (img->pixels[ny][nx].r == 255) {
                            dilate = 1;
                            break;
                        }
                    }
                }
                if (dilate)
                    break;
            }
            if (dilate) {
                output_img->pixels[y][x].r = 0;
                output_img->pixels[y][x].g = 0;
                output_img->pixels[y][x].b = 0;
            } else {
                output_img->pixels[y][x].r = 255;
                output_img->pixels[y][x].g = 255;
                output_img->pixels[y][x].b = 255;
            }
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img->pixels[y][x] = output_img->pixels[y][x];
        }
    }

    free_image(output_img);
}
