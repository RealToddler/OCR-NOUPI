#include "detectLetters.h"
#include "Image/image.h"
#include "boxes.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

cColor orange = {255, 165, 0};
cColor pink = {255, 192, 203};

// Recursive function to identify and trace a connected group of pixels
void draw_group(iImage *image, int **visited, int y, int x, int *xi, int *yi,
                int *xf, int *yf) {
  if (y < 0 || x < 0 || y >= image->height || x >= image->width ||
      visited[y][x] || is_color2(image, y, x) == 0)
    return;

  visited[y][x] = 1;

  if (x < *xi)
    *xi = x;
  if (x > *xf)
    *xf = x;
  if (y < *yi)
    *yi = y;
  if (y > *yf)
    *yf = y;

  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dy != 0 || dx != 0) {
        draw_group(image, visited, y + dy, x + dx, xi, yi, xf, yf);
      }
    }
  }
}

int is_color2(iImage *image, int y, int x) {
  if (x < 0 || y < 0 || x >= image->width || y >= image->height)
    return 0;

  pPixel pixel = image->pixels[y][x];
  if ((pixel.r == 255 && pixel.g == 255 && pixel.b == 255) ||
      (pixel.r == 255 && pixel.g == 192 && pixel.b == 203)) {
    return 0;
  }
  return 1;
}

bBoundingBox_size apply_groups_box(iImage *image) {
  int capacity = 10;
  int size = 0;
  bBoundingBox *res = malloc(capacity * sizeof(bBoundingBox));
  if (res == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  int **visited = malloc(image->height * sizeof(int *));
  if (visited == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < image->height; i++) {
    visited[i] = calloc(image->width, sizeof(int));
    if (visited[i] == NULL) {
      fprintf(stderr, "Memory allocation error[%d]\n", i);
      exit(EXIT_FAILURE);
    }
  }

  for (int y = (image->height) / 2; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      if (is_color2(image, y, x) && visited[y][x] == 0) {
        int xi = x, yi = y, xf = x, yf = y;

        draw_group(image, visited, y, x, &xi, &yi, &xf, &yf);

        if (abs(image->width - xf) <= 5) {
          xf = image->width;
        }

        if (abs(yf - yi) >= 7) {
          yi = 1;
          if (xi < 0)
            xi = 0;
          if (yi < 0)
            yi = 0;
          if (xf >= image->width)
            xf = image->width - 1;
          if (yf >= image->height)
            yf = image->height - 1;

          if (size == capacity) {
            capacity *= 2;
            res = realloc(res, capacity * sizeof(bBoundingBox));
            if (res == NULL) {
              fprintf(stderr, "Memory allocation error\n");
              exit(EXIT_FAILURE);
            }
          }

          for (int i = yi; i < yf; i++) {
            for (int j = xi; j < xf; j++) {
              visited[i][j] = 1;
            }
          }

          bBoundingBox box = {xi,
                              xf,
                              yi,
                              yf,
                              yf - yi + 1,
                              xf - xi + 1,
                              (yf - yi + 1) * (xf - xi + 1)};
          res[size++] = box;
        }
      }
    }
  }

  for (int i = 0; i < image->height; i++) {
    free(visited[i]);
  }
  free(visited);

  bBoundingBox_size result = {res, size};
  return result;
}
