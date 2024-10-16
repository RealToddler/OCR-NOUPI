#include <stdio.h>
#include <stdlib.h>
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

#include "NeuralNetwork/XNOR.h"

#include "Solver/checks.h"
#include "Solver/loadGrid.h"
#include "Solver/solver.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("An error occured while initializing SDL : %s\n",
               SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("An error occured while initializing SDL_Image : %s\n",
               IMG_GetError());
        SDL_Quit();
        return -1;
    }

    const char *path_to_preprocess[8] = {
        "preprocess_1_1.png", "preprocess_1_2.png", "preprocess_2_1.png",
        "preprocess_2_2.png", "preprocess_3_1.png", "preprocess_3_2.png",
        "preprocess_4_1.png", "preprocess_4_2.png",
    };

    const char *path_to_rotation = "rotation_2_2.png";

    int choice;

    do {
        printf("======== OCR-NOUPI ========\n");
        printf("[ 1 ] Image Preprocessing.\n");
        printf("[ 2 ] Manual Rotation.\n");
        printf("[ 3 ] Grid Detection.\n");
        printf("[ 4 ] Words List Detection.\n");
        printf("[ 5 ] Words Detection.\n");
        printf("[ 6 ] Letters Extraction.\n");
        printf("[ 7 ] NeuralNetwork - Proof of Concept (XNOR).\n");
        printf("[ 8 ] Solver.\n");
        printf("[ 9 ] Quit.\n");
        printf("\nMake your choice : ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: { // done, works fine
            printf("\nYou chose 'Image Preprocessing'\n");

            for (int i = 0; i < 8; i++) {
                char input_path[256] = "";
                char output_path[256] = "";

                strcat(input_path, "data_test/");
                strcat(input_path, path_to_preprocess[i]);

                iImage *original_img = load_image(input_path, -1);
                if (original_img == NULL) {
                    printf("original_img is NULL \n");
                    free(original_img);
                    IMG_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }
                iImage *img = resize_image(original_img, 1000, 1000);

                if (img == NULL) {
                    printf("An error occured while loading the image");
                    free(img);
                    free(original_img);
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

                strcat(output_path, "outputs/");
                strcat(output_path, path_to_preprocess[i]);

                save_image(img, output_path);
                printf("Preprocessed image saved at %s\n", output_path);

                free(original_img);
                free(img);
            }
            break;
        }
        case 2: { // done, works fine
            printf("\nYou chose 'Manual Rotation'\n");

            char input_path[256] = "";

            strcat(input_path, "data_test/");
            strcat(input_path, path_to_rotation);

            iImage *original_img = load_image(input_path, -1);
            if (original_img == NULL) {
                printf("original_img is NULL \n");
                free(original_img);
                IMG_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }
            iImage *img = resize_image(original_img, 1000, 1000);

            if (img == NULL) {
                printf("An error occured while loading the image");
                free(img);
                free(original_img);
                IMG_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }

            double angle;

            printf("Please select an angle : ");
            scanf("%lf", &angle);

            iImage *rotated_image = rotate_image(img, angle);

            if (rotated_image == NULL) {
                free(img);
                free(original_img);
                free(rotated_image);
                printf("An error occured while rotating the image.\n");
                return EXIT_FAILURE;
            }

            save_image(rotated_image, "./outputs/rotated_2_2.png");
            printf("Rotated image saved at outputs/rotation_2_2.png");

            free(img);
            free(original_img);
            free(rotated_image);
            break;
        }
        case 3: // not working rn
            // for img in test_data:
            // iImage *grid = extract_grid(img);
            // save_image(grid, path);
            break;
        case 4: // not working rn
            // for img in test_data:
            // iImage words_list = extract_words_list(img);
            // save_image(grid, path)
            break;
        case 5: // not working rn
            // for img in test_data:
            // iImage words = extract_words_list(img);
            // save_image(grid, path)
            break;
        case 6:   // not working rn
                  // arrays containing each words of a specific word list and
                  // grids function to extract letters
        case 7: { // done, works fine
            printf("\nYou chose XNOR.\n");
            XNOR();
            break;
        }
        case 8: {
            // to be fixed
            char word[256];
            char path_to_grid[256];

            printf("Please enter the path to the grid file:\n");
            scanf("%255s", path_to_grid);

            printf("Please enter the word you want to find in the grid:\n");
            scanf("%255s", word);

            solver(word, path_to_grid);
            break;
        }
        case 9: break;
        default: printf("Choice is not valid. Please try again.\n"); break;
        }
        printf("\n");
    } while (choice != 9);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
