#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wordsBuilder.h"

#include "../Image/Detection/canny.h"
#include "../Image/Detection/extract.h"
#include "../Image/image.h"
#include "../NeuralNetwork/neuralNetwork.h"
#include "../SysCommand/syscmd.h"

#include "../Image/Detection/cannyParameters.h"
#include "../Image/resize.h"

int compare_filenames_bis(const void *a, const void *b) {
    const char *fname1 = *(const char **)a;
    const char *fname2 = *(const char **)b;
    return strcmp(fname1, fname2);
}

char recognize_letter(const char *letter_full_path, NeuralNetwork *nn) {
    iImage *letter_image = load_image(letter_full_path, -1);
    if (letter_image == NULL) {
        err(EXIT_FAILURE, "couldnt load image");
    }

    double input[INPUTS_NUMBER];
    int idx = 0;
    for (int y = 0; y < letter_image->height; y++) {
        for (int x = 0; x < letter_image->width; x++) {
            Uint8 pixel_value = letter_image->pixels[y][x].r;
            input[idx++] = pixel_value > 0 ? 1.0 : 0.0;
        }
    }

    forward_pass(nn, input);

    int predicted_label = 0;
    double max_output = nn->output_layer[0];
    for (int j = 1; j < OUTPUTS_NUMBER; j++) {
        if (nn->output_layer[j] > max_output) {
            max_output = nn->output_layer[j];
            predicted_label = j;
        }
    }

    char letter = (predicted_label < 26) ? ('A' + predicted_label)
                                         : ('a' + (predicted_label - 26) - 32);

    free_image(letter_image);

    return letter;
}

void process_letters_in_word(NeuralNetwork *nn, FILE *words_file) {
    const char *letters_directory = "extracted/word_letters";
    DIR *letters_dir = opendir(letters_directory);
    if (!letters_dir) {
        err(EXIT_FAILURE, "Couldn't open directory %s", letters_directory);
    }

    char **letter_filenames = NULL;
    int letter_file_count = 0;

    struct dirent *letter_entry;
    while ((letter_entry = readdir(letters_dir)) != NULL) {
        if (strstr(letter_entry->d_name, ".png")) {
            char *filename = strdup(letter_entry->d_name);
            if (!filename) {
                err(EXIT_FAILURE, "strdup error");
            }
            char **temp_list = realloc(
                letter_filenames, (letter_file_count + 1) * sizeof(char *));
            if (!temp_list) {
                err(EXIT_FAILURE, "realloc error");
            }
            letter_filenames = temp_list;
            letter_filenames[letter_file_count++] = filename;
        }
    }
    closedir(letters_dir);
    qsort(letter_filenames, letter_file_count, sizeof(char *),
          compare_filenames_bis);

    char word_buffer[MAX_LINE_LENGTH];
    int word_buffer_index = 0;

    for (int i = 0; i < letter_file_count; i++) {
        char letter_full_path[PATH_MAX];
        snprintf(letter_full_path, sizeof(letter_full_path), "%s/%s",
                 letters_directory, letter_filenames[i]);

        char letter = recognize_letter(letter_full_path, nn);
        word_buffer[word_buffer_index++] = letter;
    }

    word_buffer[word_buffer_index] = '\0';

    fprintf(words_file, "%s\n", word_buffer);

    for (int i = 0; i < letter_file_count; i++) {
        free(letter_filenames[i]);
    }
    free(letter_filenames);

    sys_cmd("rm -rf extracted/word_letters/*");
}

void process_word_image(const char *full_path, NeuralNetwork *nn,
                        FILE *words_file) {
    iImage *image = load_image(full_path, -1);
    if (image == NULL) {
        fprintf(stderr, "Error while loading image %s\n", full_path);
        return;
    }

    iImage *resized = resize_image(image, 2000, 150);

    apply_canny(find_letters_in_word, resized);

    save_image(resized, "test.png");

    cColor pink = {255, 192, 203};

    extract_image((iImage *)load_image("test.png", -1), pink);

    process_letters_in_word(nn, words_file);

    free_image(image);
}

void process_all_word_images(NeuralNetwork *nn) {
    const char *words_directory = "extracted/words";
    DIR *dir = opendir(words_directory);
    if (!dir) {
        err(EXIT_FAILURE, "Couldn't open directory %s", words_directory);
    }

    FILE *words_file = fopen("extracted/txt_data/words.txt", "w");
    if (!words_file) {
        err(EXIT_FAILURE, "Couldn't open words.txt");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".png")) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", words_directory,
                     entry->d_name);

            printf("%s\n", full_path);
            process_word_image(full_path, nn, words_file);
        }
    }

    closedir(dir);
    fclose(words_file);
}

int words_builder() {
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

    process_all_word_images(nn);
    // free_neural_network(nn);

    return 0;
}
