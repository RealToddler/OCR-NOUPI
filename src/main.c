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
#include "UserInterface/application.h"


/*
    Its name talks for itself
*/
int create_directory(const char *path) {
    char cmd[512];

    snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", path);

    int ret = system(cmd);

    if (ret != 0) {
        fprintf(stderr, "Error creating directory: %s\n", path);
        return 0;
    }
    return 1;
}

char **split_line(const char *line) {
    if (line == NULL) {
        return NULL;
    }

    int max_tokens = 1;
    for (const char *p = line; *p != '\0'; p++) {
        if (*p == ' ') {
            max_tokens++;
        }
    }

    char **tokens = malloc((max_tokens + 1) * sizeof(char *));
    if (tokens == NULL) {
        perror("malloc failed");
        return NULL;
    }

    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        perror("strdup failed");
        free(tokens);
        return NULL;
    }

    int token_count = 0;
    char *token = strtok(line_copy, " ");
    while (token != NULL) {
        tokens[token_count++] = strdup(token);
        token = strtok(NULL, " ");
    }

    tokens[token_count] = NULL;

    free(line_copy);
    return tokens;
}

void free_tokens(char **tokens) {
    if (tokens == NULL)
        return;

    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

void bubbleSortGrids(char **grid1, char **grid2, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        char **splitted1 = split_line(grid1[i]);
        char *splitted2 = strdup(grid2[i]);

        for (int pass = 0; pass < cols - 1; pass++) {
            for (int j = 0; j < cols - pass - 1; j++) {
                int x1 = 0, y1 = 0;
                int x2 = 0, y2 = 0;
                sscanf(splitted1[j], "(%d,%d)", &x1, &y1);
                sscanf(splitted1[j + 1], "(%d,%d)", &x2, &y2);

                if (x1 > x2) {
                    char *temp1 = splitted1[j];
                    splitted1[j] = splitted1[j + 1];
                    splitted1[j + 1] = temp1;

                    char temp2 = splitted2[j];
                    splitted2[j] = splitted2[j + 1];
                    splitted2[j + 1] = temp2;
                }
            }
        }

        size_t size1 = 1;
        for (int k = 0; k < cols; k++) {
            size1 += strlen(splitted1[k]) + 1;
        }

        free(grid1[i]);
        grid1[i] = malloc(size1);
        if (!grid1[i]) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }

        grid1[i][0] = '\0';
        for (int k = 0; k < cols; k++) {
            strcat(grid1[i], splitted1[k]);
            if (k < cols - 1) {
                strcat(grid1[i], " ");
            }
        }

        free(grid2[i]);
        grid2[i] = strdup(splitted2);

        free_tokens(splitted1);
        free(splitted2);
    }
}

int main(int argc, char **agrv) {
  return open_app();
}

/*
int main() {
    /*
    if (init_SDL() == 1) {
        err(EXIT_FAILURE, "an error occured while initialising SDL");
>>>>>>> soutenance-2
    }
    char* word2="RRR";
    char* path =
"/Users/lucasbigot/Documents/EPITA/spe/OCR/OCR-NOUPI/src/inputs/level_1_image_1.png";

<<<<<<< HEAD
    return 0;
=======
    iImage *img =
<<<<<<< HEAD
        load_image("/Users/emilien/Downloads/level_1_image_1.png", -1);

    iImage *res =
        load_image("/Users/emilien/Downloads/level_1_image_1.png", -1);
=======
        load_image(path, -1);

    iImage *res =
        load_image(path, -1);
>>>>>>> lucas_soutenance_2

    if (img == NULL || res == NULL) {
        fprintf(stderr, "cant charge entry file\n");
        IMG_Quit();
        SDL_Quit();
        free(img);
        free(res);
        return EXIT_FAILURE;
    }

    // preprocess step
    grayscale(img);
    sauvola_threshold(img, 16);

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
        fprintf(stderr, "no grid file found\n");
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

<<<<<<< HEAD
    // sort grids
    bubbleSortGrids(coords->grid, letters->grid, coords->rows, coords->cols);
;

    int x_grid = 0, y_grid = 0;
    sscanf(grid_filename, "gridfile_%d_%d", &x_grid, &y_grid);
=======
    // temp
    const char *word = strdup(word2);
    cCoords wordCoords = solver((char *)word, "extracted/txt_data/letters.txt");


    int x_grid = 0, y_grid = 0;
    sscanf(grid_filename, "grid_%d_%d", &y_grid, &x_grid);
>>>>>>> lucas_soutenance_2

    FILE *file = fopen("extracted/txt_data/words.txt", "r");
    char word[1024];
    if (file != NULL) {
        while (fgets(word, sizeof(word), file)) {
            word[strcspn(word, "\n")] = '\0';

            cCoords wordCoords = solver(word, letters);

<<<<<<< HEAD
            tTuple t1 = wordCoords.t1;
            tTuple t2 = wordCoords.t2;

            int t1_x = 0, t1_y = 0, t2_x = 0, t2_y = 0;

            if ((t1.x != -1 && t2.x != -1) && (t1.y != -1 && t2.y != -1)) {

                sscanf(get_val(coords, t1.y, t1.x), "(%d,%d)", &t1_x, &t1_y);
                sscanf(get_val(coords, t2.y, t2.x), "(%d,%d)", &t2_x, &t2_y);

                if (t1.y == t2.y)
                {
                    if (t2_x - t1_x < 0) {
                        int temp1 = t1_x;
                        int temp2 = t1_y;
                        t1_x = t2_x;
                        t1_y = t2_y;
                        t2_x = temp1;
                        t2_y = temp2;
                    }
                    int last_l = (t2_x - t1_x) / ((strlen(word) - 1));
                    draw_rectangle(res, t1_x + x_grid, t1_y + y_grid,
                                   t2_x + last_l + x_grid, t2_y + y_grid + 25,
                                   orange);
                } else if (t1.x == t2.x) {
                    if (t2_y - t1_y < 0) {
                        int temp1 = t1_x;
                        int temp2 = t1_y;
                        t1_x = t2_x;
                        t1_y = t2_y;
                        t2_x = temp1;
                        t2_y = temp2;
                    }
                    int last_l = (t2_y - t1_y) / ((strlen(word) - 1));
                    draw_rectangle(res, t1_x + x_grid, t1_y + y_grid,
                                   t2_x + x_grid, t2_y + y_grid + last_l,
                                   orange);
                } else {
                    if (t2_y - t1_y < 0) {
                        int temp1 = t1_x;
                        int temp2 = t1_y;
                        t1_x = t2_x;
                        t1_y = t2_y;
                        t2_x = temp1;
                        t2_y = temp2;
                    }
                    if (t2_x < t1_x) {
                        draw_diagonal(res, t1_x + x_grid, t1_y + y_grid,
                                      t2_x + x_grid, t2_y + y_grid, orange,
                                      strlen(word), 2);
                    } else {

                        draw_diagonal(res, t1_x + x_grid, t1_y + y_grid,
                                      t2_x + x_grid, t2_y + y_grid, orange,
                                      strlen(word), 1);
                    }
                }
=======
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
            to_red_func(res, t1_x + x_grid+last_l, t1_y + y_grid, t2_x+ x_grid,
t2_y+ y_grid); } else if (t1.x == t2.x) { // vertical if (t2_y - t1_y<0) { int
temp1 = t1_x; int temp2 = t1_y; t1_x = t2_x; t1_y = t2_y; t2_x = temp1; t2_y =
temp2;
            }
            int last_l = (t2_y - t1_y) / ((strlen(word) - 1));

            to_red_func(res, t1_x + x_grid+ 15, t1_y + y_grid, t1_x + x_grid+
15, t2_y + y_grid);

            draw_rectangle(res, t1_x+ 15 + x_grid, t1_y + y_grid, t2_x+ 15 +
x_grid, t2_y + y_grid +last_l, orange); } else { if (t2_y - t1_y<0) { int temp1
= t1_x; int temp2 = t1_y; t1_x = t2_x; t1_y = t2_y; t2_x = temp1; t2_y = temp2;
>>>>>>> lucas_soutenance_2
            }

            printf("(%d, %d), (%d, %d)\n", t1_x + x_grid, t1_y + y_grid,
                   t2_x + x_grid, t2_y + y_grid);
        }
        fclose(file);
    }

    save_image(res, "output.png");

    free_image(img);
    free_image(grid);
    free(grid_filename);
    free_grid(coords);
    free_grid(letters);

    IMG_Quit();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
*/