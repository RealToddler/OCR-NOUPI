#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "../Image/image.h"

// Neural network parameters
#define IMAGE_SIZE 32
#define INPUTS_NUMBER (IMAGE_SIZE * IMAGE_SIZE)
#define HIDDEN_NODES_NUMBER 32
#define OUTPUTS_NUMBER 52 // Number of letters in the English alphabet

// Neural network structure
typedef struct
{
    double *hidden_layer;
    double *output_layer;

    double *hidden_layer_bias;
    double *output_layer_bias;

    double **hidden_weights;
    double **output_weights;
} NeuralNetwork;


NeuralNetwork *create_neural_network();
void free_neural_network(NeuralNetwork *nn);
void forward_pass(NeuralNetwork *nn, double *input_layer);
void backpropagation(NeuralNetwork *nn, double *input_layer, double *expected_output, double lr);

#endif // NEURALNETWORK_H
