#include "smallGroupReduction.h"
#include "../Image/image.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int len_group(iImage *image, int **visited, int y, int x);
void clear(iImage *image, int y, int x);

int is_color(iImage *image, int y, int x) {
    if (x < 0 || y < 0 || x >= image->width || y >= image->height)
        return 0;

    if (image->pixels[y][x].r == 255 && image->pixels[y][x].g == 255 &&
        image->pixels[y][x].b == 255)
        return 0;
    else
        return 1;
}

void apply_groups_reduction(iImage *image) {

    int **visited = malloc(image->height * sizeof(int *));
    if (visited == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < image->height; i++) {
        visited[i] =
            calloc(image->width,
                   sizeof(int)); // set visited to 0/false for each pixels.
        if (visited[i] == NULL) {
            fprintf(stderr, "Memory allocation error[%d]\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (x < 0 || y < 0 || x >= image->width || y >= image->height) {
                return;
            }

            if (is_color(image, y, x) == 1 && visited[y][x] == 0) {
                // int len = len_group(image, visited, y, x);
                int len = len_group(image, visited, y, x);

                if (len < 70 || len > 1600) {
                    // printf("len = %d\n", len);
                    // printf("x = %d\n", x);
                    // printf("y = %d\n", y);
                    clear(image, y, x);
                }
            }
        }
    }

    for (int i = 0; i < image->height; i++) {
        free(visited[i]);
    }
    free(visited);
}

void clear(iImage *image, int y, int x) {

    if (x < 0 || y < 0 || x >= image->width || y >= image->height ||
        is_color(image, y, x) == 0)
        return;

    pPixel *pixel = &image->pixels[y][x];

    pixel->r = 255;
    pixel->g = 255;
    pixel->b = 255;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++)

        {
            if (i == 0 && j == 0) {
                continue;
            }
            if (x + j >= 0 && y + i >= 0 && x + j < image->width &&
                y + i < image->height && is_color(image, y + i, x + j) == 1) {

                if (is_color(image, i + y, j + x) == 1)
                    clear(image, i + y, j + x);
            }
        }
    }
}

int len_group(iImage *image, int **visited, int y, int x) {

    if (y < 0 || x < 0 || y >= image->height || x >= image->width ||
        visited[y][x] || is_color(image, y, x) == 0)
        return 0;

    visited[y][x] = 1; // pixel is visited

    // init len =1
    int len = 1;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (x >= 0 && y >= 0 && x < image->width && y < image->height &&
                is_color(image, y + i, x + j) == 1) {
                len += len_group(image, visited, y + i, x + j);
            }
        }
    }
    return len;
}