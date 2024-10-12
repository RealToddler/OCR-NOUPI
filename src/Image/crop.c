#include "crop.h"
#include "../Image/image.h"
#include <stdio.h>
#include <stdlib.h>

iImage *crop_image(iImage *img) {
    const unsigned int width = img->width;
    const unsigned int height = img->height;

    unsigned int max_x = 0;
    unsigned int max_y = 0;

    unsigned int min_x = width - 1;
    unsigned int min_y = height - 1;

    // Rechercher les bords de la zone à conserver (pixels non blancs)
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            pPixel *pixel = &img->pixels[y][x];

            // Si le pixel n'est pas blanc, ajuster les bords
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

    // Créer l'image rognée avec les nouvelles dimensions
    iImage *cropped_img = create_image(new_width, new_height, img->path);
    if (cropped_img == NULL) {
        printf("Erreur lors de la création de l'image rognée.\n");
        return NULL;
    }

    // Copier les pixels de la zone à conserver
    for (unsigned int y = 0; y < new_height; y++) {
        for (unsigned int x = 0; x < new_width; x++) {
            cropped_img->pixels[y][x] = img->pixels[y + min_y][x + min_x];
        }
    }

    return cropped_img;
}
