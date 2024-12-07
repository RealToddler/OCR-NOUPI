#include "sauvola.h"
#include "Image/image.h"
#include <math.h>

/*
This function implements Sauvola's method with a non-adaptative threshold
https://www.lrde.epita.fr/~theo/papers/geraud.2014.ijdar.pdf
*/
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
        for (unsigned int x = bx; x < bx + block_size && x < width; x++) {
          pPixel *pixel = &img->pixels[y][x];
          unsigned int intensity = pixel->r;
          sum += intensity;
          sum_sq += intensity * intensity;
          count++;
        }
      }

      double mean = sum / (double)count;
      double variance = (sum_sq - (sum * sum) / (double)count) / (double)count;
      if (variance < 0)
        variance = 0;
      double std_dev = sqrt(variance);

      double threshold = mean * (1 + k * ((std_dev / R) - 1));

      for (unsigned int y = by; y < by + block_size && y < height; y++) {
        for (unsigned int x = bx; x < bx + block_size && x < width; x++) {
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

/*
This function implements Sauvola's method with an adaptative threshold
https://www.lrde.epita.fr/~theo/papers/geraud.2014.ijdar.pdf
*/
void sauvola_threshold_adaptative(iImage *img, const unsigned int block_size) {
  unsigned int width = img->width;
  unsigned int height = img->height;
  double k = 0.5;
  double R = 128.0;

  double global_mean = 0;
  unsigned int pixel_count = width * height;

  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      global_mean += img->pixels[y][x].r;
    }
  }
  global_mean /= pixel_count;

  for (unsigned int by = 0; by < height; by += block_size) {
    for (unsigned int bx = 0; bx < width; bx += block_size) {
      unsigned int sum = 0;
      unsigned int sum_sq = 0;
      unsigned int count = 0;

      for (unsigned int y = by; y < by + block_size && y < height; y++) {
        for (unsigned int x = bx; x < bx + block_size && x < width; x++) {
          unsigned int intensity = img->pixels[y][x].r;
          sum += intensity;
          sum_sq += intensity * intensity;
          count++;
        }
      }

      double mean = sum / (double)count;
      double variance = (sum_sq - (sum * sum) / (double)count) / (double)count;
      if (variance < 0)
        variance = 0;
      double std_dev = sqrt(variance);
      double threshold =
          global_mean + k * (mean - global_mean) * (1 + ((std_dev / R) - 1));

      for (unsigned int y = by; y < by + block_size && y < height; y++) {
        for (unsigned int x = bx; x < bx + block_size && x < width; x++) {
          if (img->pixels[y][x].r > threshold) {
            img->pixels[y][x].r = 255;
            img->pixels[y][x].g = 255;
            img->pixels[y][x].b = 255;
          } else {
            img->pixels[y][x].r = 0;
            img->pixels[y][x].g = 0;
            img->pixels[y][x].b = 0;
          }
        }
      }
    }
  }
}