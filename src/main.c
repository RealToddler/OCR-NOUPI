#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
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

#include "Image/Detection/boxes.h"
#include "Image/Detection/canny.h"
#include "Image/Detection/cannyParameters.h"
#include "Image/Detection/extract.h"

#include "Image/Preprocess/Rotation/automaticRotation.h"
#include "Image/Preprocess/Rotation/manualRotation.h"

#include "NeuralNetwork/XNOR.h"

#include "Solver/checks.h"
#include "Solver/gridBuilder.h"
#include "Solver/loadGrid.h"
#include "Solver/solver.h"
#include "Solver/wordsBuilder.h"

#include "Files/findGrid.h"

int main() {
    if (init_SDL() == 1) {
        err(EXIT_FAILURE, "an error occured while initialising SDL");
    }
    char* word2="LTM";
    char* path = "/Users/lucasbigot/Documents/EPITA/spe/OCR/OCR-NOUPI/src/inputs/level_1_image_1.png";

    iImage *img =
        load_image(path, -1);

    iImage *res =
        load_image(path, -1);

    if (img == NULL || res == NULL) {
        fprintf(stderr, "Impossible de charger l'image d'entrée.\n");
        IMG_Quit();
        SDL_Quit();
        free(img);
        free(res);
        return EXIT_FAILURE;
    }

    // preprocess step
    grayscale(img);
    sauvola_threshold(img, 16);

    save_image(img, "temp.png");

    // detections
    // functions to extract everything properly
    cColor red = {255, 0, 0};
    cColor blue = {0, 0, 255};
    cColor cyan = {43, 255, 255};
    cColor orange = {255, 165, 0};

    // detect and extract grid
    apply_canny(find_grid, img);
    extract_image(img, red);

    // detect and extract words
    apply_canny(find_word_lists, img);
    extract_image(img, blue);

    const char *directory = "extracted/";
    char *grid_filename = find_grid_file(directory);

    if (grid_filename == NULL) {
        fprintf(stderr, "Aucun fichier de grille trouvé dans le répertoire.\n");
        free_image(img);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    char grid_path[1024];
    snprintf(grid_path, sizeof(grid_path), "%s%s", directory, grid_filename);

    iImage *grid = load_image(grid_path, -1);
    if (grid == NULL) {
        fprintf(stderr, "Cant load the grid : %s\n", grid_path);
        free_image(img);
        free(grid_filename);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // detect and extract letters from grid
    apply_canny(all, grid);
    extract_image(grid, cyan);

    // build txt data files
    if (grid_builder() == 1) {
        err(EXIT_FAILURE, "an error occurend while building out the grid");
    }

    if (words_builder() == 1) {
        err(EXIT_FAILURE,
            "an error occurend while building out the words list");
    }

    gGrid *coords = load_grid("extracted/txt_data/coordinates.txt");
    gGrid *letters = load_grid("extracted/txt_data/letters.txt");
    if (coords == NULL || letters == NULL) {
        fprintf(stderr, "Cant load txt files\n");
        free_image(img);
        free_image(grid);
        free(grid_filename);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // temp
    const char *word = strdup(word2);
    cCoords wordCoords = solver((char *)word, "extracted/txt_data/letters.txt");
    

    int x_grid = 0, y_grid = 0;
    sscanf(grid_filename, "grid_%d_%d", &y_grid, &x_grid);

    tTuple t1 = wordCoords.t1;
    tTuple t2 = wordCoords.t2;

    int t1_x = 0, t1_y = 0, t2_x = 0, t2_y = 0;

    printf("t1: (%d, %d), t2: (%d, %d)\n", t1.x, t1.y, t2.x, t2.y);

    // need to know the direction (horizontal, vertical diagonal)
    // in order to know to know to which coords we should apply the correction

    if ((t1.x != -1 && t2.x != -1) && (t1.y != -1 && t2.y != -1)) {
        
        printf("in func\n");
        sscanf(get_val(coords, t1.y, t1.x), "(%d,%d)", &t1_x, &t1_y);
        sscanf(get_val(coords, t2.y, t2.x), "(%d,%d)", &t2_x, &t2_y);
        

        if (t1.y == t2.y) // horizontal
        {
            printf("horizontal");
            if (t2_x - t1_x<0) {
                int temp1 = t1_x;
                int temp2 = t1_y;
                t1_x = t2_x;
                t1_y = t2_y;
                t2_x = temp1;
                t2_y = temp2;
            }
            int last_l = (t2_x - t1_x) / ((strlen(word) - 1));
            draw_rectangle(res, t1_x + x_grid, t1_y + y_grid,
                           t2_x +last_l+ x_grid ,
                           t2_y + y_grid + 25, orange);
        } else if (t1.x == t2.x) { // vertical
            if (t2_y - t1_y<0) {
                int temp1 = t1_x;
                int temp2 = t1_y;
                t1_x = t2_x;
                t1_y = t2_y;
                t2_x = temp1;
                t2_y = temp2;
            }
            int last_l = (t2_y - t1_y) / ((strlen(word) - 1));
            draw_rectangle(res, t1_x + x_grid, t1_y + y_grid, t2_x + x_grid ,
                           t2_y + y_grid +last_l, orange);
        } else {
            if (t2_y - t1_y<0) {
                int temp1 = t1_x;
                int temp2 = t1_y;
                t1_x = t2_x;
                t1_y = t2_y;
                t2_x = temp1;
                t2_y = temp2;
            }
            printf("in else");
            if (t2_x < t1_x) {
                draw_diagonal(res, t1_x + x_grid, t1_y + y_grid,
                                   t2_x + x_grid, t2_y + y_grid, orange,
                                   strlen(word), 2);
            } else {

                draw_diagonal(res, t1_x + x_grid, t1_y + y_grid, t2_x + x_grid,
                              t2_y + y_grid, orange, strlen(word), 1);
            }
        }
    }
    save_image(res, "output.png");
    save_image(grid, "2nd.png");

    free_image(img);
    free_image(grid);
    free(grid_filename);
    free_grid(coords);
    free_grid(letters);

    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
