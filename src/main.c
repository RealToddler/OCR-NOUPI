#include <string.h>

#include "Image/Preprocess/ColorTreatment/binary.h"
#include "Image/Preprocess/ColorTreatment/grayscale.h"
#include "Image/Preprocess/ColorTreatment/invertColors.h"

#include "Image/Preprocess/NoiseReduction/otsu.h"
#include "Image/Preprocess/NoiseReduction/contrasts.h"
#include "Image/Preprocess/NoiseReduction/denoise.h"

#include "Image/resize.h"
#include "Image/image.h"
#include "Image/crop.h"

#include "Image/Detection/Letters/letters.h"

#include "Image/Preprocess/Rotation/manualRotation.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <path_to_image> <output_image>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Erreur d'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // temp params
    double angle = -42.0;

    // Charger l'image + resize
    iImage *img =resize_image((load_image(argv[1])), 32, 32);
    iImage *temp_img;

    if (img != NULL)
    {
        printf("Image chargée avec succès : %dx%d pixels\n", img->width, img->height);

        if (angle != -42)
        {
            temp_img = rotate_image(img, angle);
            img = temp_img;
        }

        // grayscale(img);
        // binary(img); // besoins d'une condition qui test si l image est toute blance ou non
        // increase_contrast(img, 1);
        // apply_gaussian_blur(img);
        // otsu_threshold(img);
        // invert_colors(img);
        // apply_canny(img);


        save_image(img,  argv[2]);
        free_image(img);
    }

    IMG_Quit();
    SDL_Quit();

    return 0;
}
