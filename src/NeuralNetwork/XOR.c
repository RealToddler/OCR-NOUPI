#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// neural network that can learn XOR function

double sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}

double d_sigmoid(double x)
{
    return x * (1 - x);
}


double init_weights()
{
    return ((double)rand()) / ((double)RAND_MAX);
}

void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + (rand() / (RAND_MAX / (n - i) + 1));
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

#define INPUTS_NUMBER 2
#define HIDDEN_NODES_NUMBER 2
#define OUTPUTS_NUMBER 1
#define TRAINING_SETS_NUMBER 4

int main(void)
{
    const double lr = 0.1f;

    double hidden_layer[HIDDEN_NODES_NUMBER];
    double output_layer[OUTPUTS_NUMBER];

    double hidden_layer_bias[HIDDEN_NODES_NUMBER];
    double output_layer_bias[OUTPUTS_NUMBER];

    double hidden_weights[INPUTS_NUMBER][HIDDEN_NODES_NUMBER];
    double output_weights[HIDDEN_NODES_NUMBER][OUTPUTS_NUMBER];

    double training_inputs[TRAINING_SETS_NUMBER][INPUTS_NUMBER] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f}};

    double training_outputs[TRAINING_SETS_NUMBER][OUTPUTS_NUMBER] = {
        {0.0f},
        {1.0f},
        {1.0f},
        {0.0f}};

    for (int i = 0; i < INPUTS_NUMBER; i++)
    {
        for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
        {
            hidden_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++)
    {
        hidden_layer_bias[i] = init_weights();
        for (int j = 0; j < OUTPUTS_NUMBER; j++)
        {
            output_weights[i][j] = init_weights();
        }
    }

    for (int i = 0; i < OUTPUTS_NUMBER; i++)
    {
        output_layer_bias[i] = init_weights();
    }

    int training_set_order[] = {0, 1, 2, 3};

    int number_of_epochs = 10000;

    // train the neural network for a number of epochs
    for (int epoch = 0; epoch < number_of_epochs; epoch++)
    {
        shuffle(training_set_order, TRAINING_SETS_NUMBER);

        for (int x = 0; x < TRAINING_SETS_NUMBER; x++)
        {
            int i = training_set_order[x];

            // Forward pass

            // Compute hidden layers activation
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
            {
                double activation = hidden_layer_bias[j];
                for (int k = 0; k < INPUTS_NUMBER; k++)
                {
                    activation += training_inputs[i][k] * hidden_weights[k][j];
                }
                hidden_layer[j] = sigmoid(activation);
            }

            // Compute output layers activation
            for (int j = 0; j < OUTPUTS_NUMBER; j++)
            {
                double activation = output_layer_bias[j];
                for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
                {
                    activation += hidden_layer[k] * output_weights[k][j];
                }
                output_layer[j] = sigmoid(activation);
            }

            printf("Input:%f %f Output:%g    Expected Output: %g\n",
                   training_inputs[i][0], training_inputs[i][1],
                   output_layer[0], training_outputs[i][0]);

            // Back propagation
            // Compute change in output weights

            double delta_output[OUTPUTS_NUMBER];

            for (int j = 0; j < OUTPUTS_NUMBER; j++)
            {
                double error = (training_outputs[i][j] - output_layer[j]);
                delta_output[j] = error * d_sigmoid(output_layer[j]);
            }

            // Compute change in hidden weights
            double delta_hidden[HIDDEN_NODES_NUMBER];
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
            {
                double error = 0.0f;
                for (int k = 0; k < OUTPUTS_NUMBER; k++)
                {
                    error += delta_output[k] * output_weights[j][k];
                }
                delta_hidden[j] = error * d_sigmoid(hidden_layer[j]);
            }

            // Apply changes in output weights
            for (int j = 0; j < OUTPUTS_NUMBER; j++)
            {
                output_layer_bias[j] += delta_output[j] * lr;
                for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
                {
                    output_weights[k][j] += hidden_layer[k] * delta_output[j] * lr;
                }
            }

            // Apply changes in hidden weights
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
            {
                hidden_layer_bias[j] += delta_hidden[j] * lr;
                for (int k = 0; k < INPUTS_NUMBER; k++)
                {
                    hidden_weights[k][j] += training_inputs[i][k] * delta_hidden[j] * lr;
                }
            }
        }
    }

    // Print final weights after training
    fputs("Final Hidden Weights\n[ ", stdout);
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
    {
        fputs("[ ", stdout);
        for (int k = 0; k < INPUTS_NUMBER; k++)
        {
            printf("%f ", hidden_weights[k][j]);
        }
        fputs("] ", stdout);
    }

    fputs("]\nFinal Hidden Biases\n[ ", stdout);
    for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
    {
        printf("%f ", hidden_layer_bias[j]);
    }

    fputs("]\nFinal Output Weights", stdout);
    for (int j = 0; j < OUTPUTS_NUMBER; j++)
    {
        fputs("[ ", stdout);
        for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
        {
            printf("%f ", output_weights[k][j]);
        }
        fputs("]\n", stdout);
    }

    fputs("Final Output Biases\n[ ", stdout);
    for (int j = 0; j < OUTPUTS_NUMBER; j++)
    {
        printf("%f ", output_layer_bias[j]);
    }

    fputs("]\n", stdout);

    return 0;
}
