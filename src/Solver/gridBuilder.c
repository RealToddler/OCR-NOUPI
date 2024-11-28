#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Files/sortCoordsFile.h"
#include "../NeuralNetwork/neuralNetwork.h"
#include "../SysCommand/syscmd.h"
#include "gridBuilder.h"

#define MAX_LINE_LENGTH 1024 // Added definition for MAX_LINE_LENGTH

int compare_file_entries(const void *a, const void *b) {
    FileEntry *entryA = (FileEntry *)a;
    FileEntry *entryB = (FileEntry *)b;

    if (entryA->ycoords != entryB->ycoords)
        return entryA->ycoords - entryB->ycoords;
    else
        return entryA->xcoords - entryB->xcoords;
}

int is_empty_line(const char *line) {
    for (int i = 0; line[i] != '\0'; i++) {
        if (!isspace(line[i])) {
            return 0;
        }
    }
    return 1;
}

void remove_empty_lines(const char *input_filename,
                        const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (!input_file) {
        err(EXIT_FAILURE, "could not load file");
    }

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        fclose(input_file);
        err(EXIT_FAILURE, "could not load output file");
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), input_file)) {
        if (!is_empty_line(line)) {
            fputs(line, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

int extract_coordinates(const char *filename, int *x, int *y) {
    return sscanf(filename, "%d_%d", x, y);
}

int compare_files(const void *a, const void *b) {
    FileEntry *file1 = (FileEntry *)a;
    FileEntry *file2 = (FileEntry *)b;

    if (file1->ycoords == file2->ycoords) {
        return file1->xcoords - file2->xcoords;
    }
    return file1->ycoords - file2->ycoords;
}

FileEntry *process_files(const char *directory, int *file_count) {
    DIR *dir;
    struct dirent *entry;
    FileEntry *file_list = NULL;
    *file_count = 0;

    dir = opendir(directory);
    if (!dir) {
        err(EXIT_FAILURE, "couldn't open directory");
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".png")) {
            int xcoords = 0, ycoords = 0;
            if (extract_coordinates(entry->d_name, &xcoords, &ycoords) != 2) {
                continue;
            }

            FileEntry *temp_list =
                realloc(file_list, (*file_count + 1) * sizeof(FileEntry));
            if (!temp_list) {
                err(EXIT_FAILURE, "realloc error");
                free(file_list);
                closedir(dir);
                return NULL;
            }
            file_list = temp_list;

            file_list[*file_count].filename = strdup(entry->d_name);
            if (!file_list[*file_count].filename) {
                err(EXIT_FAILURE, "strdup error");
                free(file_list);
                closedir(dir);
                return NULL;
            }
            file_list[*file_count].xcoords = xcoords;
            file_list[*file_count].ycoords = ycoords;
            (*file_count)++;
        }
    }

    closedir(dir);
    qsort(file_list, *file_count, sizeof(FileEntry), compare_files);

    return file_list;
}

void reconstruct_grid(const char *directory, FileEntry *file_list,
                      int file_count, NeuralNetwork *nn) {
    // Sort the file_list based on ycoords and then xcoords to ensure proper order
    qsort(file_list, file_count, sizeof(FileEntry), compare_file_entries);

    FILE *letters_file = fopen("extracted/txt_data/temp_letters.txt", "w");
    if (!letters_file) {
        err(EXIT_FAILURE, "Couldn't open temp_letters.txt");
    }

    FILE *coordinates_file =
        fopen("extracted/txt_data/temp_coordinates.txt", "w");
    if (!coordinates_file) {
        fclose(letters_file);
        err(EXIT_FAILURE, "Couldn't open temp_coordinates.txt");
    }

    // Corrected loop start index from 0 to 1 to avoid out-of-bounds access
    for (int i = 1; i < file_count; i++) {
        if (abs(file_list[i - 1].ycoords - file_list[i].ycoords) < 15) {
            file_list[i].ycoords = file_list[i - 1].ycoords;
        }
    }

    int *processed = calloc(file_count, sizeof(int));
    if (!processed) {
        fclose(letters_file);
        fclose(coordinates_file);
        err(EXIT_FAILURE, "Calloc error");
    }

    int line_start_idx = 0;
    while (line_start_idx < file_count) {
        int first_in_line = 1;
        int line_end_idx = line_start_idx;

        // Group files with similar ycoords (same line)
        while (line_end_idx < file_count &&
               abs(file_list[line_end_idx].ycoords -
                   file_list[line_start_idx].ycoords) <= 10) {
            line_end_idx++;
        }

        for (int idx = line_start_idx; idx < line_end_idx; idx++) {
            if (processed[idx]) {
                continue;
            }

            size_t path_length =
                strlen(directory) + strlen(file_list[idx].filename) + 2;
            char *full_path = malloc(path_length);
            if (!full_path) {
                free(processed);
                fclose(letters_file);
                fclose(coordinates_file);
                err(EXIT_FAILURE, "Malloc error");
            }

            snprintf(full_path, path_length, "%s/%s", directory,
                     file_list[idx].filename);

            iImage *image = load_image(full_path, -1);
            if (image == NULL) {
                free(full_path);
                continue;
            }

            int input_size = image->height * image->width;
            double *input = malloc(input_size * sizeof(double));
            if (!input) {
                free(full_path);
                // Free image resources
                for (int t = 0; t < image->height; t++) {
                    free(image->pixels[t]);
                }
                free(image->pixels);
                free(image->path);
                free(image);
                err(EXIT_FAILURE, "Malloc error for input");
            }

            int idx_input = 0;
            for (int i = 0; i < image->height; i++) {
                for (int j = 0; j < image->width; j++) {
                    Uint8 pixel_value = image->pixels[i][j].r;
                    input[idx_input++] = pixel_value > 0 ? 1.0 : 0.0;
                }
            }

            forward_pass(nn, input);

            int predicted_label = 0;
            double max_output = nn->output_layer[0];
            for (int i = 1; i < OUTPUTS_NUMBER; i++) {
                if (nn->output_layer[i] > max_output) {
                    max_output = nn->output_layer[i];
                    predicted_label = i;
                }
            }

            char letter = (predicted_label < 26)
                              ? ('A' + predicted_label)
                              : ('a' + (predicted_label - 26) - 32);

            fputc(letter, letters_file);

            if (!first_in_line) {
                fputc(' ', coordinates_file);
            }

            fprintf(coordinates_file, "(%d,%d)", file_list[idx].xcoords,
                    file_list[idx].ycoords);
            first_in_line = 0;

            // Free resources
            for (int t = 0; t < image->height; t++) {
                free(image->pixels[t]);
            }
            free(image->pixels);
            free(image->path);
            free(image);
            free(full_path);
            free(input);

            processed[idx] = 1;
        }

        fputc('\n', letters_file);
        fputc('\n', coordinates_file);

        line_start_idx = line_end_idx;
    }

    fclose(letters_file);
    fclose(coordinates_file);
    free(processed);
}


int grid_builder() {
    srand((unsigned int)time(NULL));

    NeuralNetwork *nn = create_neural_network();

    FILE *file_check = fopen("NeuralNetwork/neural_network_weights.dat", "rb");
    if (file_check) {
        fclose(file_check);
        load_neural_network(nn, "NeuralNetwork/neural_network_weights.dat");
    } else {
        printf("Couldn't load the weights file\n");
        return EXIT_FAILURE;
    }

    const char *directory = "extracted/grid_letters";
    int file_count;
    FileEntry *file_list = process_files(directory, &file_count);

    if (!file_list || file_count == 0) {
        free(file_list);
        free_neural_network(nn);
        printf("Empty directory. No letters to process.\n");
        return EXIT_FAILURE;
    }

    // Adjusted file creation commands
    sys_cmd("touch extracted/txt_data/temp_letters.txt");
    sys_cmd("touch extracted/txt_data/temp_coordinates.txt");
    sys_cmd("touch extracted/txt_data/letters.txt");
    // sys_cmd("touch extracted/txt_data/coords.txt");

    sys_cmd("touch extracted/txt_data/coordinates.txt");
   // sys_cmd("touch extracted/txt_data/test.txt");

    reconstruct_grid(directory, file_list, file_count, nn);

    remove_empty_lines("extracted/txt_data/temp_letters.txt",
                       "extracted/txt_data/letters.txt");

    remove_empty_lines("extracted/txt_data/temp_coordinates.txt",
                       "extracted/txt_data/coordinates.txt");

    sys_cmd("rm extracted/txt_data/temp_letters.txt");
    sys_cmd("rm extracted/txt_data/temp_coordinates.txt");

    // sort_pairs_in_file("extracted/txt_data/coordinates.txt", "extracted/txt_data/coords.txt", "extracted/txt_data/test.txt");

        // Free allocated memory
        for (int i = 0; i < file_count; i++) {
        free(file_list[i].filename);
    }
    free(file_list);
    free_neural_network(nn);

    return 0;
}