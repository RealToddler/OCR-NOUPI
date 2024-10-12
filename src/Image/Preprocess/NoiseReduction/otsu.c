#include "../Image/image.h"

unsigned int compute_otsu_threshold(unsigned int *histogram,
                                    unsigned int total_pixels) {
    double sum = 0;
    for (int t = 0; t < 256; t++)
        sum += t * histogram[t];

    double sumB = 0;
    unsigned int wB = 0;
    unsigned int wF = 0;

    double varMax = 0;
    unsigned int threshold = 0;

    for (int t = 0; t < 256; t++) {
        wB += histogram[t];
        if (wB == 0)
            continue;

        wF = total_pixels - wB;
        if (wF == 0)
            break;

        sumB += t * histogram[t];

        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double varBetween = (double)wB * (double)wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t;
        }
    }
    return threshold;
}

void otsu_threshold(iImage *img, const unsigned int block_size) {
    unsigned int width = img->width;
    unsigned int height = img->height;

    for (unsigned int by = 0; by < height; by += block_size) {
        for (unsigned int bx = 0; bx < width; bx += block_size) {
            unsigned int histogram[256] = {0};
            unsigned int count = 0;

            for (unsigned int y = by; y < by + block_size && y < height; y++) {
                for (unsigned int x = bx; x < bx + block_size && x < width;
                     x++) {
                    pPixel *pixel = &img->pixels[y][x];
                    histogram[pixel->r]++;
                    count++;
                }
            }

            unsigned int threshold = compute_otsu_threshold(histogram, count);

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
