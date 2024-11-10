#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Image/Preprocess/ColorTreatment/binary.h"
#include "Image/Preprocess/ColorTreatment/grayscale.h"
#include "Image/Preprocess/ColorTreatment/invertColors.h"

#include "Image/Preprocess/NoiseReduction/contrasts.h"
#include "Image/Preprocess/NoiseReduction/gaussianBlur.h"
#include "Image/Preprocess/NoiseReduction/otsu.h"
#include "Image/Preprocess/NoiseReduction/smallGroupReduction.h"

#include "Image/crop.h"
#include "Image/image.h"
#include "Image/resize.h"

#include "Image/Detection/canny.h"
#include "Image/Detection/cannyParameters.h"
#include "Image/Detection/extract.h"

#include "Image/Preprocess/Rotation/manualRotation.h"

#include "NeuralNetwork/XNOR.h"

#include "Solver/checks.h"
#include "Solver/loadGrid.h"
#include "Solver/solver.h"

/*
    Its name talks for itself
*/
int create_directory(const char *path) {
    char cmd[512];

    snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", path);

    int ret = system(cmd);

    if (ret != 0) {
        fprintf(stderr, "Error creating directory: %s\n", path);
        return -1;
    }

    return 0;
}

int main() {
    create_directory("../outputs");
    create_directory("../outputs/detections/letters");
    create_directory("../outputs/detections/words");
    create_directory("../outputs/detections/grid");
    create_directory("../outputs/preprocess");

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

    const char *path_to_rotation = "rotation.png";

    const char *path_to_images[8] = {
        "1_1.png", "1_2.png", "2_1.png", "2_2.png",
        "3_1.png", "3_2.png", "4_1.png", "4_2.png",
    };

    int choice;

    do {
        printf("======== OCR-NOUPI ========\n");
        printf("[ 1 ] Image Preprocessing.\n");
        printf("[ 2 ] Manual Rotation.\n");
        printf("[ 3 ] Grid Detection.\n");
        printf("[ 4 ] Words Detection.\n");
        printf("[ 5 ] Letters Extraction.\n");
        printf("[ 6 ] NeuralNetwork - Proof of Concept (XNOR).\n");
        printf("[ 7 ] Solver.\n");
        printf("[ 8 ] Quit.\n");
        printf("\nMake your choice : ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: { // done, works fine
            printf("\nYou chose 'Image Preprocessing'\n");

            for (int i = 0; i < 8; i++) {
                char input_path[256] = "";
                char output_path[256] = "";

                strcat(input_path, "../data_test/preprocess/");
                strcat(input_path, path_to_images[i]);

                iImage *original_img = load_image(input_path, -1);
                if (original_img == NULL) {
                    printf("original_img is NULL\n");
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
                otsu_threshold(img, 32);
                // binary(img);
                apply_groups_reduction(img);
                // invert_colors(img);

                strcat(output_path, "../outputs/preprocess/");
                strcat(output_path, path_to_images[i]);

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

            strcat(input_path, "../data_test/preprocess/");
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

            grayscale(img);
            apply_gaussian_blur(img);
            increase_contrast(img, 10, 8);
            otsu_threshold(img, 32);
            // binary(img);
            apply_groups_reduction(img);
            // invert_colors(img);

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

            apply_groups_reduction(rotated_image);

            save_image(rotated_image, "../outputs/rotated.png");
            printf("Rotated image saved at ../outputs/rotated.png");

            free(img);
            free(original_img);
            free(rotated_image);
            break;
        }
        case 3: { // 4/8
            printf("\nYou chose Grid Detection.\n");

            for (int i = 0; i < 8; i++) {
                char input_path[256] = "";
                char output_path[256] = "";

                strcat(input_path, "../data_test/detections/grid/");
                strcat(input_path, path_to_images[i]);

                iImage *img = load_image(input_path, -1);
                if (img == NULL) {
                    printf("img is NULL\n");
                    free(img);
                    IMG_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }

                // binary(img);

                apply_canny(find_grid, img);

                strcat(output_path, "../outputs/detections/grid/");
                strcat(output_path, path_to_images[i]);

                save_image(img, output_path);
                free(img);
            }

            printf("Grid detection is done.\n");
            break;
        }
        case 4: { // 4/8
            printf("\nYou chose Words Detection.\n");

            for (int i = 0; i < 8; i++) {
                char input_path[256] = "";
                char output_path[256] = "";

                strcat(input_path, "../data_test/detections/words/");
                strcat(input_path, path_to_images[i]);

                iImage *img = load_image(input_path, -1);
                if (img == NULL) {
                    printf("img is NULL\n");
                    free(img);
                    IMG_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }

                // binary(img);

                apply_canny(find_word_lists, img);

                strcat(output_path, "../outputs/detections/words/");
                strcat(output_path, path_to_images[i]);

                save_image(img, output_path);
                free(img);
            }

            printf("Words List detection is done.\n");
            break;
        }
        case 5: { // done, good enough (6/8)
            printf("\nYou chose Letters Extraction.\n");

            for (int i = 0; i < 8; i++) {
                char input_path[256] = "";
                char output_path[256] = "";

                strcat(input_path, "../data_test/detections/letters/");
                strcat(input_path, path_to_images[i]);

                iImage *original_img = load_image(input_path, -1);
                if (original_img == NULL) {
                    printf("original_img is NULL\n");
                    free(original_img);
                    IMG_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }
                iImage *img = resize_image(original_img, 2000, 400);

                if (img == NULL) {
                    printf("An error occured while loading the image");
                    free(img);
                    free(original_img);
                    IMG_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }

                binary(img);

                apply_canny(find_letters_in_word, img);

                extract_image(img, i);

                strcat(output_path, "../outputs/detections/letters/");
                strcat(output_path, path_to_images[i]);

                save_image(img, output_path);

                free(original_img);
                free(img);
            }

            printf("Letters extraction is done.\n");
            break;
        }
        case 6: { // done, works fine
            printf("\nYou chose XNOR.\n");
            XNOR();
            break;
        }
        case 7: { // done, allows continuous word checking
            char word[256];

            while (1) {
                printf("\nPlease enter the word you want to find in the grid "
                       "(or type 'exit' to stop): ");
                scanf("%255s", word);
                if (strcmp(word, "exit") == 0) {
                    break;
                }
                solver(word, "../data_test/grid.txt");
            };

            break;
        }
        case 8: break;
        default: printf("Choice is not valid. Please try again.\n"); break;
        }
        printf("\n");
    } while (choice != 8);

    IMG_Quit();
    SDL_Quit();

    return 0;
}