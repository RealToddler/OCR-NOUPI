#include <stdlib.h>
#include "neuralNetwork.h"
#include "formulas.h"

// Create and initialize the neural network
NeuralNetwork *create_neural_network()
{
    NeuralNetwork *nn = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));

    // Allocate memory for layers
    nn->hidden_layer = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    nn->output_layer = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

    // Allocate memory for biases
    nn->hidden_layer_bias = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    nn->output_layer_bias = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

    // Allocate memory for weights
    nn->hidden_weights = (double **)malloc(sizeof(double *) * INPUTS_NUMBER);
    for (int i = 0; i < INPUTS_NUMBER; i++)
    {
        nn->hidden_weights[i] = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    }

    nn->output_weights = (double **)malloc(sizeof(double *) * HIDDEN_NODES_NUMBER);
    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++)
    {
        nn->output_weights[i] = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);
    }

    // Initialize weights and biases
    for (int i = 0; i < INPUTS_NUMBER; i++)
    {
        for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
        {
            nn->hidden_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++)
    {
        nn->hidden_layer_bias[i] = init_weights();
        for (int j = 0; j < OUTPUTS_NUMBER; j++)
        {
            nn->output_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < OUTPUTS_NUMBER; i++)
    {
        nn->output_layer_bias[i] = init_weights();
    }

    return nn;
}

// Free the neural network memory
void free_neural_network(NeuralNetwork *nn)
{
    // Free layers
    free(nn->hidden_layer);
    free(nn->output_layer);

    // Free biases
    free(nn->hidden_layer_bias);
    free(nn->output_layer_bias);

    // Free weights
    for (int i = 0; i < INPUTS_NUMBER; i++)
    {
        free(nn->hidden_weights[i]);
    }
    free(nn->hidden_weights);

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++)
    {
        free(nn->output_weights[i]);
    }
    free(nn->output_weights);

    // Free the neural network structure
    free(nn);
}

// Forward pass function
void forward_pass(NeuralNetwork *nn, double *input_layer)
{
    // Compute activations for hidden layer
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
    {
        double activation = nn->hidden_layer_bias[j];
        for (int k = 0; k < INPUTS_NUMBER; k++)
        {
            activation += input_layer[k] * nn->hidden_weights[k][j];
        }
        nn->hidden_layer[j] = sigmoid(activation);
    }

    // Compute activations for output layer
    for (int j = 0; j < OUTPUTS_NUMBER; j++)
    {
        double activation = nn->output_layer_bias[j];
        for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
        {
            activation += nn->hidden_layer[k] * nn->output_weights[k][j];
        }
        nn->output_layer[j] = activation; // No activation yet
    }

    // Apply softmax activation
    softmax(nn->output_layer, OUTPUTS_NUMBER);
}

// Backpropagation function
void backpropagation(NeuralNetwork *nn, double *input_layer, double *expected_output, double lr)
{
    // Calculate output layer deltas
    double delta_output[OUTPUTS_NUMBER];
    for (int j = 0; j < OUTPUTS_NUMBER; j++)
    {
        delta_output[j] = nn->output_layer[j] - expected_output[j];
    }

    // Calculate hidden layer deltas
    double delta_hidden[HIDDEN_NODES_NUMBER];
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
    {
        double error = 0.0;
        for (int k = 0; k < OUTPUTS_NUMBER; k++)
        {
            error += delta_output[k] * nn->output_weights[j][k];
        }
        delta_hidden[j] = error * d_sigmoid(nn->hidden_layer[j]);
    }

    // Update weights and biases for output layer
    for (int j = 0; j < OUTPUTS_NUMBER; j++)
    {
        nn->output_layer_bias[j] -= delta_output[j] * lr;
        for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
        {
            nn->output_weights[k][j] -= nn->hidden_layer[k] * delta_output[j] * lr;
        }
    }

    // Update weights and biases for hidden layer
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
    {
        nn->hidden_layer_bias[j] -= delta_hidden[j] * lr;
        for (int k = 0; k < INPUTS_NUMBER; k++)
        {
            nn->hidden_weights[k][j] -= input_layer[k] * delta_hidden[j] * lr;
        }
    }
}
