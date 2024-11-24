#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "../NeuralNetwork/neuralNetwork.h"

#define MAX_LINE_LENGTH 1024

typedef struct {
    char *filename;
    int xcoords;
    int ycoords;
} FileEntry;

int is_empty_line(const char *line) {
    for (int i = 0; line[i] != '\0'; i++) {
        if (!isspace(line[i])) {
            return 0; // Ligne non vide
        }
    }
    return 1; // Ligne vide
}

int extract_coordinates(const char *filename, int *x, int *y) {
    return sscanf(filename, "%d_%d", x, y);
}

int compare_files(const void *a, const void *b) {
    FileEntry *file1 = (FileEntry *)a;
    FileEntry *file2 = (FileEntry *)b;

    // Comparaison par ycoords, puis xcoords
    if (file1->ycoords == file2->ycoords) {
        return file1->xcoords - file2->xcoords;
    }
    return file1->ycoords - file2->ycoords;
}

FileEntry *list_and_sort_png_files(const char *directory, int *file_count) {
    DIR *dir;
    struct dirent *entry;
    FileEntry *file_list = NULL;
    *file_count = 0;

    dir = opendir(directory);
    if (!dir) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".png")) {
            int xcoords = 0, ycoords = 0;
            if (extract_coordinates(entry->d_name, &xcoords, &ycoords) != 2) {
                continue; // Ignorer les fichiers au format incorrect
            }

            FileEntry *temp_list =
                realloc(file_list, (*file_count + 1) * sizeof(FileEntry));
            if (!temp_list) {
                perror("realloc");
                free(file_list);
                closedir(dir);
                return NULL;
            }
            file_list = temp_list;

            file_list[*file_count].filename = strdup(entry->d_name);
            if (!file_list[*file_count].filename) {
                perror("strdup");
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

void reconstruct_grid_and_write_to_file(const char *directory, FileEntry *file_list, int file_count, NeuralNetwork *nn) {
    // Déterminer la taille maximale des y (lignes)
    int max_y = 0;
    for (int i = 0; i < file_count; i++) {
        if (file_list[i].ycoords > max_y) {
            max_y = file_list[i].ycoords;
        }
    }

    // Écrire la grille dans un fichier
    FILE *output_file = fopen("results.txt", "w");
    if (!output_file) {
        perror("fopen");
        return;
    } 

    // Parcourir les lignes (y)
    for (int y = 0; y <= max_y; y++) {
        for (int file_idx = 0; file_idx < file_count; file_idx++) {
            if (file_list[file_idx].ycoords == y) {
                // Charger l'image
                size_t path_length =
                    strlen(directory) + strlen(file_list[file_idx].filename) + 2;
                char *full_path = malloc(path_length);
                if (!full_path) {
                    fprintf(stderr, "Erreur d'allocation mémoire pour le chemin complet.\n");
                    continue;
                }

                snprintf(full_path, path_length, "%s/%s", directory, file_list[file_idx].filename);

                iImage *test_image = load_image(full_path, -1);
                if (test_image == NULL) {
                    printf("Échec du chargement de l'image de test : %s\n", full_path);
                    free(full_path);
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

                char letter = (predicted_label < 26) ? ('A' + predicted_label)
                                                     : ('a' + (predicted_label - 26) - 32);

                // Écrire la lettre dans le fichier
                fputc(letter, output_file);

                // Libérer l'image de test
                for (int t = 0; t < test_image->height; t++) {
                    free(test_image->pixels[t]);
                }
                free(test_image->pixels);
                free(test_image->path);
                free(test_image);
                free(full_path);
            }
        }
        fputc('\n', output_file);
    }

    fclose(output_file);
}

#include <stdio.h>
#include <stdlib.h>

void remove_empty_lines(const char *input_filename,
                        const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (!input_file) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        fclose(input_file);
        return;
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

int f(void) {
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

    // Créer le réseau neuronal
    NeuralNetwork *nn = create_neural_network();

    // Charger les poids du réseau neuronal si disponibles
    FILE *file_check = fopen("../NeuralNetwork/neural_network_weights.dat", "rb");
    if (file_check) {
        fclose(file_check);
        load_neural_network(nn, "../NeuralNetwork/neural_network_weights.dat");
    } else {
        printf("Aucun fichier de poids trouvé, réseau non initialisé avec des poids.\n");
    }

    // Liste les fichiers PNG dans le répertoire spécifié
    const char *directory = "../extracted/grid_letters";
    int file_count;
    FileEntry *file_list = list_and_sort_png_files(directory, &file_count);

    if (!file_list || file_count == 0) {
        fprintf(stderr, "Aucun fichier PNG trouvé ou erreur lors de la lecture du répertoire.\n");
        free(file_list);
        free_neural_network(nn);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Reconstruire la grille et écrire les résultats dans un fichier
    reconstruct_grid_and_write_to_file(directory, file_list, file_count, nn);

    remove_empty_lines("results.txt", "result.txt");
    // Libérer la mémoire
    for (int i = 0; i < file_count; i++) {
        free(file_list[i].filename);
    }
    free(file_list);
    free_neural_network(nn);

    // Quitter SDL2
    IMG_Quit();
    SDL_Quit();

    return 0;
}
