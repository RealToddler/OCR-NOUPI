#include <string.h>

#include "Image/Preprocess/ColorTreatment/binary.h"
#include "Image/Preprocess/ColorTreatment/grayscale.h"
#include "Image/Preprocess/ColorTreatment/invertColors.h"

#include "Image/Preprocess/NoiseReduction/contrasts.h"
#include "Image/Preprocess/NoiseReduction/gaussianBlur.h"
#include "Image/Preprocess/NoiseReduction/otsu.h"

#include "Image/crop.h"
#include "Image/image.h"
#include "Image/resize.h"

#include "Image/Detection/canny.h"
#include "Image/Detection/extract.h"

#include "Image/Preprocess/Rotation/manualRotation.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("An error occured while initializing SDL : %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("An error occured while initializing SDL_Image : %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    int choice;

    do {
        printf("======== OCR-NOUPI ========\n");
        printf("[ 1 ] Image Preprocessing.\n");
        printf("[ 2 ] Manual Rotation.\n");
        printf("[ 3 ] Grid Detection.\n");
        printf("[ 4 ] Words List Detection.\n")
        printf("[ 5 ] Words Detection.\n");
        printf("[ 6 ] Letters Extraction.\n");
        printf("[ 7 ] NeuralNetwork - Proof of Concept (XNOR).\n");
        printf("[ 8 ] Solver.\n")
        printf("[ 9 ]  Quit.\n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("You chose  'Image Preprocessing'\n");
            
            iImage *img = resize_image(load_image(argv[1], -1), 1000, 1000);

            if (img == NULL) {
                printf("An error occured while loading the image");
                free(img);
                IMG_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }

            grayscale(img);
            apply_gaussian_blur(img);
            increase_contrast(img, 10, 8);
            binary(img);
            otsu_threshold(img, 32);
            invert_colors(img);
            save_image(img, "outputs/preprocessed_image.png");
            printf("Preprocessed image saved at outputs/preprocessed_image.png");

            free(img);
            break;
        case 2:
            printf("You chose 'Manual Rotation'\n");

            iImage *img = resize_image(load_image(argv[1], -1), 1000, 1000);

            if (img == NULL) {
                printf("An error occured while loading the image");
                free(img);
                IMG_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }

            save_image(img, "outputs/preprocessed_image.png");
            printf("Rotated images saved at outputs/rotated_image_<i>.png");
            
            free(img);
            break;
        case 3: 
            // function that detects the grid
            break;
        case 4: 
            // function that detects a word list
            break;
        case 5: 
            // arrays containing words list of a specific grid
            // funciton to extract words
            break;
        case 6: 
            // arrays containing each words of a specific word list and grids
            // function to extract letters
        case 7: 
            // XNOR();
            break;
        case 8: 
            // char* word;
            // char* path_to_grid;
            // solver(word, path_to_grid);
            break;
        case 9: break;
        default: printf("Choice is not valid. Please try again.\n"); break;
        }
        printf("\n");
    } while (choice != 9);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
