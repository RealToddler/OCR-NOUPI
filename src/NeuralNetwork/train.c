#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Image/image.h"
#include "formulas.h"
#include "neuralNetwork.h"
#include "train.h"

// Function to load images from dataset directory
int load_dataset(const char *dataset_path, iImage **images) {
    int image_count = 0;
    for (size_t i = 0; i < 2; i++) {
        char letter_dir[512];
        for (char letter = 'A'; letter <= 'Z'; letter++) {
            if (i == 0) {
                snprintf(letter_dir, sizeof(letter_dir), "%s/%c", dataset_path,
                         letter);
            } else {
                snprintf(letter_dir, sizeof(letter_dir), "%s/%c1", dataset_path,
                         letter + 32);
            }

            DIR *dir = opendir(letter_dir);
            if (dir == NULL) {
                printf("Could not open directory %s\n", letter_dir);
                continue;
            }

            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0)
                    continue;

                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s/%s", letter_dir,
                         entry->d_name);

                int label;
                if (i == 0) {
                    label = letter - 'A';
                } else {
                    label = 26 + (letter - 'A');
                }
                iImage *image = load_image(filepath, label);
                if (image != NULL) {
                    images[image_count++] = image;
                    if (image_count >= MAX_IMAGES) {
                        closedir(dir);
                        return image_count;
                    }
                }
            }
            closedir(dir);
        }
    }

    return image_count;
}

// Shuffle the dataset
void shuffle_images(iImage **images, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        iImage *temp = images[i];
        images[i] = images[j];
        images[j] = temp;
    }
}

int training() {
    srand((unsigned int)time(NULL));

    const double lr = 0.1f;

    NeuralNetwork *nn = create_neural_network();

    FILE *file_check = fopen(WEIGHTS_FILE, "rb");
    if (file_check) {
        fclose(file_check);
        load_neural_network(nn, WEIGHTS_FILE);
    } else {
        iImage *images[MAX_IMAGES];
        int total_images = load_dataset("dataset", images);
        if (total_images == 0) {
            printf("Aucune image chargée.\n");
            return -1;
        }
        printf("Chargé %d images.\n", total_images);

        // Boucle d'entraînement
        for (int epoch = 0; epoch < EPOCHS; epoch++) {
            // Mélanger les images
            shuffle_images(images, total_images);

            for (int x = 0; x < total_images; x++) {
                double input_layer[INPUTS_NUMBER];
                int idx = 0;
                for (int i = 0; i < images[x]->height; i++) {
                    for (int j = 0; j < images[x]->width; j++) {
                        Uint8 pixel_value = images[x]->pixels[i][j].r;
                        input_layer[idx++] = pixel_value > 0 ? 1.0 : 0.0;
                    }
                }

                double expected_output[OUTPUTS_NUMBER] = {0};
                expected_output[images[x]->label] = 1.0;

                forward_pass(nn, input_layer);

                backpropagation(nn, input_layer, expected_output, lr);
            }

            printf("Époque %d/%d terminée.\n", epoch + 1, EPOCHS);
        }

        save_neural_network(nn, WEIGHTS_FILE);

        for (int i = 0; i < total_images; i++) {
            for (int y = 0; y < images[i]->height; y++) {
                free(images[i]->pixels[y]);
            }
            free(images[i]->pixels);
            free(images[i]->path);
            free(images[i]);
        }
    }

    while (1) {
        char test_image_path[256];
        printf("Entrez le chemin vers une image de test (ou 'exit' pour "
               "quitter) : ");
        scanf("%s", test_image_path);

        if (strcmp(test_image_path, "exit") == 0) {
            break;
        }

        iImage *test_image = load_image(test_image_path, -1);
        if (test_image == NULL) {
            printf("Échec du chargement de l'image de test.\n");
            continue;
        }

        double test_input[INPUTS_NUMBER];
        int idx = 0;
        for (int i = 0; i < test_image->height; i++) {
            for (int j = 0; j < test_image->width; j++) {
                Uint8 pixel_value = test_image->pixels[i][j].r;
                test_input[idx++] = pixel_value > 0 ? 1.0 : 0.0;
            }
        }

        forward_pass(nn, test_input);

        int predicted_label = 0;
        double max_output = nn->output_layer[0];
        for (int i = 1; i < OUTPUTS_NUMBER; i++) {
            if (nn->output_layer[i] > max_output) {
                max_output = nn->output_layer[i];
                predicted_label = i;
            }
        }

        if (predicted_label < 26) {
            printf("Lettre prédite : %c\n", 'A' + predicted_label);
        } else {
            printf("Lettre prédite : %c\n", 'a' + (predicted_label - 26));
        }

        for (int y = 0; y < test_image->height; y++) {
            free(test_image->pixels[y]);
        }
        free(test_image->pixels);
        free(test_image->path);
        free(test_image);
    }

    free_neural_network(nn);

    IMG_Quit();
    SDL_Quit();

    return 1;
}