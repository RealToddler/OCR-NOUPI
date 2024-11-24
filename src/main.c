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
#include "Image/Preprocess/NoiseReduction/sauvola.h"
#include "Image/Preprocess/NoiseReduction/smallGroupReduction.h"

#include "Image/crop.h"
#include "Image/image.h"
#include "Image/resize.h"

#include "Image/Detection/canny.h"
#include "Image/Detection/cannyParameters.h"
#include "Image/Detection/extract.h"
#include "Image/Detection/boxes.h"

#include "Image/Preprocess/Rotation/automaticRotation.h"
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
    /*
    create_directory("../outputs");
    create_directory("../outputs/detections/letters");
    create_directory("../outputs/detections/words");
    create_directory("../outputs/detections/grid");
    create_directory("../outputs/preprocess");
    */

    // 1 Init whatever needs to be instantiate :

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

    // 2 : Apply preprocess to the image

    iImage *img =
        load_image("/Users/emilien/Downloads/level_1_image_1.png", -1);

    grayscale(img);
    sauvola_threshold(img, 16);
    // dilate_image(img, 1);

    // 3 : Detections


    cColor red = {255, 0, 0};
    cColor blue = {0, 0, 255};
    cColor cyan = {43, 255, 255};

    // detect and extract grid
    apply_canny(find_grid, img);
    extract_image(img, 0, red);

    // detect and extracts letters from grid
    iImage *grid = load_image("extracted/grid.png", -1);
    apply_canny(all, grid);
    save_image(grid, "gridimg.png");
    extract_image(grid, 0, cyan);

    // detect and extracts words
    apply_canny(find_word_lists, img);
    extract_image(img, 0, blue);

    // ?? : Save results and free memories
    save_image(img, "output.png");

    IMG_Quit();
    SDL_Quit();

    return 0;
}