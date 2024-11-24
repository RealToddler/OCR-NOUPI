#include "neuralNetwork.h"
#include "formulas.h"
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#include "../Image/image.h"

// Create and initialize the neural network
NeuralNetwork *create_neural_network() {
    NeuralNetwork *nn = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));

    // Allocate memory for layers
    nn->hidden_layer = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    nn->output_layer = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

    // Allocate memory for biases
    nn->hidden_layer_bias =
        (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    nn->output_layer_bias = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

    // Allocate memory for weights
    nn->hidden_weights = (double **)malloc(sizeof(double *) * INPUTS_NUMBER);
    for (int i = 0; i < INPUTS_NUMBER; i++) {
        nn->hidden_weights[i] =
            (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    }

    nn->output_weights =
        (double **)malloc(sizeof(double *) * HIDDEN_NODES_NUMBER);
    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
        nn->output_weights[i] =
            (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);
    }

    // Initialize weights and biases
    for (int i = 0; i < INPUTS_NUMBER; i++) {
        for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
            nn->hidden_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
        nn->hidden_layer_bias[i] = init_weights();
        for (int j = 0; j < OUTPUTS_NUMBER; j++) {
            nn->output_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < OUTPUTS_NUMBER; i++) {
        nn->output_layer_bias[i] = init_weights();
    }

    return nn;
}

// Free the neural network memory
void free_neural_network(NeuralNetwork *nn) {
    // Free layers
    free(nn->hidden_layer);
    free(nn->output_layer);

    // Free biases
    free(nn->hidden_layer_bias);
    free(nn->output_layer_bias);

    // Free weights
    for (int i = 0; i < INPUTS_NUMBER; i++) {
        free(nn->hidden_weights[i]);
    }
    free(nn->hidden_weights);

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
        free(nn->output_weights[i]);
    }
    free(nn->output_weights);

    // Free the neural network structure
    free(nn);
}

// Forward pass function
void forward_pass(NeuralNetwork *nn, double *input_layer) {
    // Compute activations for hidden layer
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
        double activation = nn->hidden_layer_bias[j];
        for (int k = 0; k < INPUTS_NUMBER; k++) {
            activation += input_layer[k] * nn->hidden_weights[k][j];
        }
        nn->hidden_layer[j] = sigmoid(activation);
    }

    // Compute activations for output layer
    for (int j = 0; j < OUTPUTS_NUMBER; j++) {
        double activation = nn->output_layer_bias[j];
        for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
            activation += nn->hidden_layer[k] * nn->output_weights[k][j];
        }
        nn->output_layer[j] = activation; // No activation yet
    }

    // Apply softmax activation
    softmax(nn->output_layer, OUTPUTS_NUMBER);
}

// Backpropagation function
void backpropagation(NeuralNetwork *nn, double *input_layer,
                     double *expected_output, double lr) {
    // Calculate output layer deltas
    double delta_output[OUTPUTS_NUMBER];
    for (int j = 0; j < OUTPUTS_NUMBER; j++) {
        delta_output[j] = nn->output_layer[j] - expected_output[j];
    }

    // Calculate hidden layer deltas
    double delta_hidden[HIDDEN_NODES_NUMBER];
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
        double error = 0.0;
        for (int k = 0; k < OUTPUTS_NUMBER; k++) {
            error += delta_output[k] * nn->output_weights[j][k];
        }
        delta_hidden[j] = error * d_sigmoid(nn->hidden_layer[j]);
    }

    // Update weights and biases for output layer
    for (int j = 0; j < OUTPUTS_NUMBER; j++) {
        nn->output_layer_bias[j] -= delta_output[j] * lr;
        for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
            nn->output_weights[k][j] -=
                nn->hidden_layer[k] * delta_output[j] * lr;
        }
    }

    // Update weights and biases for hidden layer
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
        nn->hidden_layer_bias[j] -= delta_hidden[j] * lr;
        for (int k = 0; k < INPUTS_NUMBER; k++) {
            nn->hidden_weights[k][j] -= input_layer[k] * delta_hidden[j] * lr;
        }
    }
}

// Fonction pour sauvegarder le réseau neuronal
void save_neural_network(NeuralNetwork *nn, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Impossible d'ouvrir le fichier pour sauvegarder les poids.\n");
        return;
    }

    // Sauvegarder les poids et les biais de la couche cachée
    // Save hidden_weights
    for (int i = 0; i < INPUTS_NUMBER; i++) {
        fwrite(nn->hidden_weights[i], sizeof(double), HIDDEN_NODES_NUMBER,
               file);
    }
    // Save hidden_layer_bias
    fwrite(nn->hidden_layer_bias, sizeof(double), HIDDEN_NODES_NUMBER, file);

    // Sauvegarder les poids et les biais de la couche de sortie
    // Save output_weights
    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
        fwrite(nn->output_weights[i], sizeof(double), OUTPUTS_NUMBER, file);
    }
    // Save output_layer_bias
    fwrite(nn->output_layer_bias, sizeof(double), OUTPUTS_NUMBER, file);

    fclose(file);
    printf("Poids sauvegardés dans le fichier %s\n", filename);
}

// Fonction pour charger le réseau neuronal
void load_neural_network(NeuralNetwork *nn, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Impossible d'ouvrir le fichier pour charger les poids.\n");
        return;
    }

    // Charger les poids et les biais de la couche cachée
    // Load hidden_weights
    for (int i = 0; i < INPUTS_NUMBER; i++) {
        fread(nn->hidden_weights[i], sizeof(double), HIDDEN_NODES_NUMBER, file);
    }
    // Load hidden_layer_bias
    fread(nn->hidden_layer_bias, sizeof(double), HIDDEN_NODES_NUMBER, file);

    // Charger les poids et les biais de la couche de sortie
    // Load output_weights
    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
        fread(nn->output_weights[i], sizeof(double), OUTPUTS_NUMBER, file);
    }
    // Load output_layer_bias
    fread(nn->output_layer_bias, sizeof(double), OUTPUTS_NUMBER, file);

    fclose(file);
    printf("Poids chargés depuis le fichier %s\n", filename);
}

char neural_predict(NeuralNetwork* nn,  char *image_path) {
    srand((unsigned int)time(NULL));

    iImage *test_image = load_image(image_path, -1);
    if (test_image == NULL) {
        printf("Échec du chargement de l'image de test.\n");
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

    // Libérer l'image de test
    for (int y = 0; y < test_image->height; y++) {
        free(test_image->pixels[y]);
    }
    free(test_image->pixels);
    free(test_image->path);
    free(test_image);

    if (predicted_label < 26) {
        return 'A' + predicted_label; // Majuscules
    } else {
        return 'a' + (predicted_label - 26); // Minuscules
    }

    return 0;
}