#include "isWhite.h"
#include "Image/image.h"

/*
    This function checks if an image is full white
*/
int isWhite(iImage *img) { // 0 = false 1 = true
  const unsigned int width = img->width;
  const unsigned int height = img->height;

  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      pPixel *pixel = &img->pixels[y][x];
      if (!(pixel->r == 255 && pixel->g == 255 && pixel->b == 255)) {
        return 0;
      }
    }
  }

  return 1;
}
