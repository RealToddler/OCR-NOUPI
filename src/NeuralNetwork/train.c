#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Image/image.h"
#include "formulas.h"
#include "neuralNetwork.h"

// Constants
#define MAX_IMAGES 15000 // Adjust based on your dataset size

// Function to load images from dataset directory
int load_dataset(const char *dataset_path, iImage **images) {
    int image_count = 0;

    // Loop over each letter directory

    for (size_t i = 0; i < 2;
         i++) // `i == 0` for uppercase and `i == 1` for lowercase + digit
    {
        char letter_dir[512];
        for (char letter = 'A'; letter <= 'Z'; letter++) {
            if (i == 0) {
                // Uppercase letters
                snprintf(letter_dir, sizeof(letter_dir), "%s/%c", dataset_path,
                         letter);
            } else {
                // Lowercase letters followed by 1 (a1, b1, etc.)
                snprintf(letter_dir, sizeof(letter_dir), "%s/%c1", dataset_path,
                         letter +
                             32); // Convert to lowercase (ASCII offset of 32)
            }

            DIR *dir = opendir(letter_dir);
            if (dir == NULL) {
                printf("Could not open directory %s\n", letter_dir);
                continue;
            }

            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                // Skip "." and ".."
                if (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0)
                    continue;

                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s/%s", letter_dir,
                         entry->d_name);

                // Load image with the correct label (adjusted for lowercase
                // with the second iteration of `i`)
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


int fooo(void) {
    srand((unsigned int)time(NULL));

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    const double lr = 0.1f;

    // Créer le réseau neuronal
    NeuralNetwork *nn = create_neural_network();

    // Vérifier si un fichier de poids existe déjà pour charger les poids
    FILE *file_check = fopen(WEIGHTS_FILE, "rb");
    if (file_check) {
        fclose(file_check);
        load_neural_network(nn, WEIGHTS_FILE);
    } else {
        // Charger le dataset
        iImage *images[MAX_IMAGES];
        int total_images = load_dataset("../../dataset", images);
        if (total_images == 0) {
            printf("Aucune image chargée.\n");
            return -1;
        }
        printf("Chargé %d images.\n", total_images);

        int number_of_epochs = 1000; // Ajustez selon vos besoins

        // Boucle d'entraînement
        for (int epoch = 0; epoch < number_of_epochs; epoch++) {
            // Mélanger les images
            shuffle_images(images, total_images);

            for (int x = 0; x < total_images; x++) {
                // Aplatir les pixels de l'image pour l'entrée
                double input_layer[INPUTS_NUMBER];
                int idx = 0;
                for (int i = 0; i < images[x]->height; i++) {
                    for (int j = 0; j < images[x]->width; j++) {
                        Uint8 pixel_value = images[x]->pixels[i][j].r;
                        input_layer[idx++] = pixel_value > 0 ? 1.0 : 0.0;
                    }
                }

                // Sortie attendue (one-hot encoding)
                double expected_output[OUTPUTS_NUMBER] = {0};
                expected_output[images[x]->label] = 1.0;

                // Forward pass
                forward_pass(nn, input_layer);

                // Backpropagation
                backpropagation(nn, input_layer, expected_output, lr);
            }

            printf("Époque %d/%d terminée.\n", epoch + 1, number_of_epochs);
        }

        // Sauvegarder les poids du réseau neuronal après l'entraînement
        save_neural_network(nn, WEIGHTS_FILE);

        // Libérer la mémoire allouée
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
        // Tester le réseau neuronal avec une nouvelle image
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

        // Forward pass
        forward_pass(nn, test_input);

        // Trouver le label prédit
        int predicted_label = 0;
        double max_output = nn->output_layer[0];
        for (int i = 1; i < OUTPUTS_NUMBER; i++) {
            if (nn->output_layer[i] > max_output) {
                max_output = nn->output_layer[i];
                predicted_label = i;
            }
        }

        if (predicted_label < 26) {
            printf("Lettre prédite : %c\n",
                   'A' + predicted_label); // Majuscules
        } else {
            printf("Lettre prédite : %c\n",
                   'a' + (predicted_label - 26)); // Minuscules
        }

        // Libérer l'image de test
        for (int y = 0; y < test_image->height; y++) {
            free(test_image->pixels[y]);
        }
        free(test_image->pixels);
        free(test_image->path);
        free(test_image);
    }

    // Libérer la mémoire du réseau neuronal
    free_neural_network(nn);

    // Quitter SDL2
    IMG_Quit();
    SDL_Quit();

    return 0;
}