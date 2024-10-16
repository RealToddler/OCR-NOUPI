#include "crop.h"
#include "../Image/image.h"
#include <stdio.h>
#include <stdlib.h>

/*
    Returns a the max-cropped image
*/
iImage *crop_image(iImage *img) {
    const unsigned int width = img->width;
    const unsigned int height = img->height;

    unsigned int max_x = 0;
    unsigned int max_y = 0;

    unsigned int min_x = width - 1;
    unsigned int min_y = height - 1;

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            pPixel *pixel = &img->pixels[y][x];

            if (!(pixel->r == 255 && pixel->g == 255 && pixel->b == 255)) {
                if (x < min_x)
                    min_x = x;
                if (y < min_y)
                    min_y = y;
                if (x > max_x)
                    max_x = x;
                if (y > max_y)
                    max_y = y;
            }
        }
    }

    unsigned int new_width = max_x - min_x + 1;
    unsigned int new_height = max_y - min_y + 1;

    iImage *cropped_img = create_image(new_width, new_height, img->path);
    if (cropped_img == NULL) {
        printf("An error occured while creating the cropped image.\n");
        return NULL;
    }

    for (unsigned int y = 0; y < new_height; y++) {
        for (unsigned int x = 0; x < new_width; x++) {
            cropped_img->pixels[y][x] = img->pixels[y + min_y][x + min_x];
        }
    }

    return cropped_img;
}
