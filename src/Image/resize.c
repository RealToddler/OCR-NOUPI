#include "Image/resize.h"
#include "Utils/image.h"
#include <stdlib.h>

iImage *resize_image(iImage *img, unsigned int new_width, unsigned int new_height)
{
    unsigned int old_width = img->width;
    unsigned int old_height = img->height;

    // Créer une nouvelle image avec les nouvelles dimensions
    iImage *resized_img = create_image(new_width, new_height, img->path);
    if (resized_img == NULL)
    {
        printf("Erreur lors de la création de l'image redimensionnée.\n");
        return NULL;
    }

    // Calculer les ratios pour mapper les pixels
    double x_ratio = (double)old_width / new_width;
    double y_ratio = (double)old_height / new_height;

    // Parcourir chaque pixel de la nouvelle image
    for (unsigned int y = 0; y < new_height; y++)
    {
        for (unsigned int x = 0; x < new_width; x++)
        {
            // Trouver les coordonnées correspondantes dans l'image originale
            unsigned int orig_x = (unsigned int)(x * x_ratio);
            unsigned int orig_y = (unsigned int)(y * y_ratio);

            // S'assurer que les indices ne dépassent pas les limites
            if (orig_x >= old_width)
                orig_x = old_width - 1;
            if (orig_y >= old_height)
                orig_y = old_height - 1;

            // Copier le pixel de l'image originale vers la nouvelle image
            resized_img->pixels[y][x] = img->pixels[orig_y][orig_x];
        }
    }

    return resized_img;
}
