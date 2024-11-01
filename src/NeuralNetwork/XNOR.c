#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// neural network that can learn XNOR function
// largely inspired from https://youtu.be/LA4I3cWkp1E?si=ysvdwESGe3dV8m0U
void softmax(double *input, double *output, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        output[i] = exp(input[i]);
        sum += output[i];
    }

    for (int i = 0; i < length; i++) {
        output[i] /= sum;
    }
}

/*
    This function inits weights of our NeuralNetwork
*/
double init_weights() {
    return ((double)rand()) / ((double)RAND_MAX);
}

/*
    This function shuffles the data of the dataset of our NeuralNetwork
*/
void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + (rand() / (RAND_MAX / (n - i) + 1));
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

#define INPUTS_NUMBER 2
#define HIDDEN_NODES_NUMBER 2
#define OUTPUTS_NUMBER 2
#define TRAINING_SETS_NUMBER 4

/*
    Main function in which the NeuralNetwork is initialized, trained and tested.
*/
void XNOR(void) {
    const double lr = 0.1f;

    double hidden_layer[HIDDEN_NODES_NUMBER];
    double output_layer[OUTPUTS_NUMBER];

    double hidden_layer_bias[HIDDEN_NODES_NUMBER];
    double output_layer_bias[OUTPUTS_NUMBER];

    double hidden_weights[INPUTS_NUMBER][HIDDEN_NODES_NUMBER];
    double output_weights[HIDDEN_NODES_NUMBER][OUTPUTS_NUMBER];

    double training_inputs[TRAINING_SETS_NUMBER][INPUTS_NUMBER] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

    double training_outputs[TRAINING_SETS_NUMBER][OUTPUTS_NUMBER] = {
        {1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}};

    for (int i = 0; i < INPUTS_NUMBER; i++) {
        for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
            hidden_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++) {
        hidden_layer_bias[i] = init_weights();
        for (int j = 0; j < OUTPUTS_NUMBER; j++) {
            output_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < OUTPUTS_NUMBER; i++) {
        output_layer_bias[i] = init_weights();
    }

    int training_set_order[] = {0, 1, 2, 3};

    int number_of_epochs = 100000;

    // train the neural network for a number of epochs
    for (int epoch = 0; epoch < number_of_epochs; epoch++) {
        shuffle(training_set_order, TRAINING_SETS_NUMBER);

        for (int x = 0; x < TRAINING_SETS_NUMBER; x++) {
            int i = training_set_order[x];

            // Forward pass

            // Compute hidden layers activation
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
                double activation = hidden_layer_bias[j];
                for (int k = 0; k < INPUTS_NUMBER; k++) {
                    activation += training_inputs[i][k] * hidden_weights[k][j];
                }
                hidden_layer[j] = fmax(0.0, activation);
            }

            // Compute output layers activation
            double output_layer_input[OUTPUTS_NUMBER];
            for (int j = 0; j < OUTPUTS_NUMBER; j++) {
                double activation = output_layer_bias[j];
                for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
                    activation += hidden_layer[k] * output_weights[k][j];
                }
                output_layer_input[j] = activation;
            }

            softmax(output_layer_input, output_layer, OUTPUTS_NUMBER);

            // Back propagation
            // Compute change in output weights

            double delta_output[OUTPUTS_NUMBER];

            for (int j = 0; j < OUTPUTS_NUMBER; j++) {
                double error = output_layer[j] - training_outputs[i][j];
                delta_output[j] = error;
            }

            // Compute change in hidden weights
            double delta_hidden[HIDDEN_NODES_NUMBER];
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
                double error = 0.0f;
                for (int k = 0; k < OUTPUTS_NUMBER; k++) {
                    error += delta_output[k] * output_weights[j][k];
                }
                delta_hidden[j] = error * (hidden_layer[j] > 0 ? 1.0 : 0.0);
            }

            // Apply changes in output weights
            for (int j = 0; j < OUTPUTS_NUMBER; j++) {
                output_layer_bias[j] -= delta_output[j] * lr;
                for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
                    output_weights[k][j] -=
                        hidden_layer[k] * delta_output[j] * lr;
                }
            }

            // Apply changes in hidden weights
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
                hidden_layer_bias[j] -= delta_hidden[j] * lr;
                for (int k = 0; k < INPUTS_NUMBER; k++) {
                    hidden_weights[k][j] -=
                        training_inputs[i][k] * delta_hidden[j] * lr;
                }
            }
        }

        if ((epoch + 1) % 10000 == 0) {
            printf("Epoch %d/%d completed\n", epoch + 1, number_of_epochs);
        }
    }

    printf("Training completed after %d epochs.\n", number_of_epochs);

    while (1) {
        double input1, input2;
        printf("Enter two inputs between 0 and 1 (or -1 to exit): ");
        if (scanf("%lf %lf", &input1, &input2) != 2 || input1 == -1 ||
            input2 == -1) {
            break;
        }

        for (int j = 0; j < HIDDEN_NODES_NUMBER; j++) {
            double activation = hidden_layer_bias[j];
            activation += input1 * hidden_weights[0][j];
            activation += input2 * hidden_weights[1][j];
            hidden_layer[j] = fmax(0.0, activation);
        }

        double output_layer_input[OUTPUTS_NUMBER];
        for (int j = 0; j < OUTPUTS_NUMBER; j++) {
            double activation = output_layer_bias[j];
            for (int k = 0; k < HIDDEN_NODES_NUMBER; k++) {
                activation += hidden_layer[k] * output_weights[k][j];
            }
            output_layer_input[j] = activation;
        }

        softmax(output_layer_input, output_layer, OUTPUTS_NUMBER);
        printf("Predicted Output : %d\n",
               output_layer[1] > output_layer[0] ? 0 : 1);
    }
}
