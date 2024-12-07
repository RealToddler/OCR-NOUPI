#include <stdlib.h>

#include "Image/image.h"
#include "resize.h"

/*
    Returns a resized image, using ratios.
*/
iImage *resize_image(iImage *img, unsigned int new_width,
                     unsigned int new_height) {
  unsigned int old_width = img->width;
  unsigned int old_height = img->height;

  iImage *resized_img = create_image(new_width, new_height, img->path);
  if (resized_img == NULL) {
    printf("An error occured creating the resized image.\n");
    return resized_img;
  }

  double x_ratio = (double)old_width / new_width;
  double y_ratio = (double)old_height / new_height;

  for (unsigned int y = 0; y < new_height; y++) {
    for (unsigned int x = 0; x < new_width; x++) {
      unsigned int orig_x = (unsigned int)(x * x_ratio);
      unsigned int orig_y = (unsigned int)(y * y_ratio);

      if (orig_x >= old_width)
        orig_x = old_width - 1;
      if (orig_y >= old_height)
        orig_y = old_height - 1;

      resized_img->pixels[y][x] = img->pixels[orig_y][orig_x];
    }
  }

  return resized_img;
}

/*
  Resize an image by filling it with white pixels
*/
iImage *resize_image_with_white(iImage *img, unsigned int new_width,
                                unsigned int new_height) {
  unsigned int old_width = img->width;
  unsigned int old_height = img->height;

  unsigned int square_size = (old_width > old_height) ? old_width : old_height;

  iImage *square_img = create_image(square_size, square_size, img->path);
  if (square_img == NULL) {
    printf("An error occurred creating the square image.\n");
    return NULL;
  }

  for (unsigned int y = 0; y < square_size; y++) {
    for (unsigned int x = 0; x < square_size; x++) {
      square_img->pixels[y][x].r = 255;
      square_img->pixels[y][x].g = 255;
      square_img->pixels[y][x].b = 255;
    }
  }

  unsigned int x_offset = (square_size - old_width) / 2;
  unsigned int y_offset = (square_size - old_height) / 2;

  for (unsigned int y = 0; y < old_height; y++) {
    for (unsigned int x = 0; x < old_width; x++) {
      square_img->pixels[y + y_offset][x + x_offset] = img->pixels[y][x];
    }
  }

  iImage *resized_img = create_image(new_width, new_height, img->path);
  if (resized_img == NULL) {
    printf("An error occurred creating the resized image.\n");
    free_image(square_img);
    return NULL;
  }

  double x_ratio = (double)square_size / new_width;
  double y_ratio = (double)square_size / new_height;

  for (unsigned int y = 0; y < new_height; y++) {
    for (unsigned int x = 0; x < new_width; x++) {
      unsigned int orig_x = (unsigned int)(x * x_ratio);
      unsigned int orig_y = (unsigned int)(y * y_ratio);

      if (orig_x >= square_size)
        orig_x = square_size - 1;
      if (orig_y >= square_size)
        orig_y = square_size - 1;

      resized_img->pixels[y][x] = square_img->pixels[orig_y][orig_x];
    }
  }

  free_image(square_img);

  return resized_img;
}
