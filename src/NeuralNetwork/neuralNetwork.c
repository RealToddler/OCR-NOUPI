#include "neuralNetwork.h"
#include "Image/image.h"
#include "formulas.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Initialize and allocate memory for the neural network
NeuralNetwork *create_neural_network() {
  NeuralNetwork *nn = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));

  nn->hidden_layer = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
  nn->output_layer = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

  nn->hidden_layer_bias =
      (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
  nn->output_layer_bias = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

  nn->hidden_weights = (double **)malloc(sizeof(double *) * INPUTS_NUMBER);
  for (int i = 0; i < INPUTS_NUMBER; i++) {
    nn->hidden_weights[i] =
        (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
  }

  nn->output_weights =
      (double **)malloc(sizeof(double *) * HIDDEN_NODES_NUMBER);
  for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
    nn->output_weights[i] = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);
  }

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

// Free all memory allocated for the neural network
void free_neural_network(NeuralNetwork *nn) {
  free(nn->hidden_layer);
  free(nn->output_layer);

  free(nn->hidden_layer_bias);
  free(nn->output_layer_bias);

  for (int i = 0; i < INPUTS_NUMBER; i++) {
    free(nn->hidden_weights[i]);
  }
  free(nn->hidden_weights);

  for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
    free(nn->output_weights[i]);
  }
  free(nn->output_weights);

  free(nn);
}

// Perform forward propagation in the neural network
void forward_pass(NeuralNetwork *nn, double *input_layer) {
  for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
    double activation = nn->hidden_layer_bias[j];
    for (int k = 0; k < INPUTS_NUMBER; k++) {
      activation += input_layer[k] * nn->hidden_weights[k][j];
    }
    nn->hidden_layer[j] = sigmoid(activation);
  }

  for (int j = 0; j < OUTPUTS_NUMBER; j++) {
    double activation = nn->output_layer_bias[j];
    for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
      activation += nn->hidden_layer[k] * nn->output_weights[k][j];
    }
    nn->output_layer[j] = activation;
  }

  softmax(nn->output_layer, OUTPUTS_NUMBER);
}

// Perform backpropagation to adjust neural network weights
void backpropagation(NeuralNetwork *nn, double *input_layer,
                     double *expected_output, double lr) {
  double delta_output[OUTPUTS_NUMBER];
  for (int j = 0; j < OUTPUTS_NUMBER; j++) {
    delta_output[j] = nn->output_layer[j] - expected_output[j];
  }

  double delta_hidden[HIDDEN_NODES_NUMBER];
  for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
    double error = 0.0;
    for (int k = 0; k < OUTPUTS_NUMBER; k++) {
      error += delta_output[k] * nn->output_weights[j][k];
    }
    delta_hidden[j] = error * d_sigmoid(nn->hidden_layer[j]);
  }

  for (int j = 0; j < OUTPUTS_NUMBER; j++) {
    nn->output_layer_bias[j] -= delta_output[j] * lr;
    for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
      nn->output_weights[k][j] -= nn->hidden_layer[k] * delta_output[j] * lr;
    }
  }

  for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
    nn->hidden_layer_bias[j] -= delta_hidden[j] * lr;
    for (int k = 0; k < INPUTS_NUMBER; k++) {
      nn->hidden_weights[k][j] -= input_layer[k] * delta_hidden[j] * lr;
    }
  }
}

// Save the neural network's weights to a file
void save_neural_network(NeuralNetwork *nn, const char *filename) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    err(EXIT_FAILURE, "couldn't open weights file");
  }

  for (int i = 0; i < INPUTS_NUMBER; i++) {
    fwrite(nn->hidden_weights[i], sizeof(double), HIDDEN_NODES_NUMBER, file);
  }
  fwrite(nn->hidden_layer_bias, sizeof(double), HIDDEN_NODES_NUMBER, file);

  for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
    fwrite(nn->output_weights[i], sizeof(double), OUTPUTS_NUMBER, file);
  }
  fwrite(nn->output_layer_bias, sizeof(double), OUTPUTS_NUMBER, file);

  fclose(file);
  printf("Weights saved in %s\n", filename);
}

// Load the neural network's weights from a file
void load_neural_network(NeuralNetwork *nn, const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    err(EXIT_FAILURE, "couldn't open weights file");
  }

  for (int i = 0; i < INPUTS_NUMBER; i++) {
    fread(nn->hidden_weights[i], sizeof(double), HIDDEN_NODES_NUMBER, file);
  }
  fread(nn->hidden_layer_bias, sizeof(double), HIDDEN_NODES_NUMBER, file);

  for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
    fread(nn->output_weights[i], sizeof(double), OUTPUTS_NUMBER, file);
  }
  fread(nn->output_layer_bias, sizeof(double), OUTPUTS_NUMBER, file);

  fclose(file);
  printf("Weights loaded from %s\n", filename);
}

// Predict the letter from an image using the neural network
char neural_predict(NeuralNetwork *nn, char *image_path) {
  srand((unsigned int)time(NULL));

  iImage *image = load_image(image_path, -1);
  if (image == NULL) {
    err(EXIT_FAILURE, "error while loading the image");
  }

  double input[INPUTS_NUMBER];
  int idx = 0;
  for (int i = 0; i < image->height; i++) {
    for (int j = 0; j < image->width; j++) {
      Uint8 pixel_value = image->pixels[i][j].r;
      input[idx++] = pixel_value > 0 ? 1.0 : 0.0;
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

  for (int y = 0; y < image->height; y++) {
    free(image->pixels[y]);
  }
  free(image->pixels);
  free(image->path);
  free(image);

  return (predicted_label < 26) ? 'A' + predicted_label
                                : 'a' + (predicted_label - 26);
}