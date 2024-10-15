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

    const char *path_to_preprocess[8] = {
        "test_data/preprocess_1_1.png",
        "test_data/preprocess_1_2.png",
        "test_data/preprocess_2_1.png",
        "test_data/preprocess_2_2.png",
        "test_data/preprocess_3_1.png",
        "test_data/preprocess_3_2.png",
        "test_data/preprocess_4_1.png",
        "test_data/preprocess_4_2.png",
    };

    const char *path_to_rotation = "test_data/rotation_2_2.png";

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
        printf("[ 9 ]  Quit.\n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("You chose  'Image Preprocessing'\n");

            for (int i = 1; i < 8; i++) {
                for (int j = 1; j < 3; i++) {
                    iImage *img =
                        resize_image(load_image(path_to_images[i], -1), 1000, 1000);

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

                    const char* output_path = "foo.png";

                    save_image(img, output_path);
                    printf("Preprocessed image saved at "
                           "outputs/%s\n", output_path);

                    free(img);
                }
            }
            break;
        case 2:
            printf("You chose 'Manual Rotation'\n");
            
            iImage *img = resize_image(load_image(path_to_rotation, -1), 1000, 1000);

            if (img == NULL) {
                printf("An error occured while loading the image");
                free(img);
                IMG_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }

            double angle;

            printf("Please select an angle :\n");
            scanf("%ld", &angle);

            rotate_image(img, angle);

            save_image(img, "outputs/preprocessed_image.png");
            printf("Rotated image saved at outputs/rotated_image_<i>.png");
            
            free(img);
            break;
        case 3: // not working rn
            // for img in test_data:
                // iImage *grid = extract_grid(img);
                // save_image(grid, path);
            break;
        case 4:  // not working rn
            // for img in test_data:
                // iImage words_list = extract_words_list(img);
                // save_image(grid, path)
            break;
        case 5: // not working rn
            // for img in test_data:
                // iImage words = extract_words_list(img);
                // save_image(grid, path)
            break;
        case 6:  // not working rn
            // arrays containing each words of a specific word list and grids
            // function to extract letters
        case 7:  // done, works fine
            XNOR();
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
