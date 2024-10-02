#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Image/image.h"
#include "neuralNetwork.h"

// Activation functions
double sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}

double d_sigmoid(double x)
{
    return x * (1 - x);
}

// Softmax function
void softmax(double *output_layer, int size)
{
    double max = output_layer[0];
    for (int i = 1; i < size; i++)
    {
        if (output_layer[i] > max)
            max = output_layer[i];
    }

    double sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        output_layer[i] = exp(output_layer[i] - max); // For numerical stability
        sum += output_layer[i];
    }

    for (int i = 0; i < size; i++)
    {
        output_layer[i] /= sum;
    }
}

// Weight initialization
double init_weights()
{
    return ((double)rand() / RAND_MAX) * 2 - 1; // Random between -1 and 1
}

// Neural network parameters
#define IMAGE_SIZE 32
#define INPUTS_NUMBER (IMAGE_SIZE * IMAGE_SIZE)
#define HIDDEN_NODES_NUMBER 24
#define OUTPUTS_NUMBER 26 // Number of letters in the English alphabet
#define MAX_IMAGES 15000   // Adjust based on your dataset size

// Function to load images from dataset directory
int load_dataset(const char *dataset_path, iImage **images)
{
    int image_count = 0;

    // Loop over each letter directory
    for (char letter = 'A'; letter <= 'Z'; letter++)
    {
        char letter_dir[256];
        snprintf(letter_dir, sizeof(letter_dir), "%s/%c", dataset_path, letter);

        DIR *dir = opendir(letter_dir);
        if (dir == NULL)
        {
            printf("Could not open directory %s\n", letter_dir);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            // Skip "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", letter_dir, entry->d_name);

            iImage *image = load_image(filepath, letter - 'A');
            if (image != NULL)
            {
                images[image_count++] = image;
                if (image_count >= MAX_IMAGES)
                {
                    closedir(dir);
                    return image_count;
                }
            }
        }
        closedir(dir);
    }

    return image_count;
}

// Shuffle the dataset
void shuffle_images(iImage **images, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        iImage *temp = images[i];
        images[i] = images[j];
        images[j] = temp;
    }
}

// Main function
int main(void)
{
    srand((unsigned int)time(NULL));

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
    {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    const double lr = 0.1f;

    // Initialize layers
    double *hidden_layer = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    double *output_layer = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

    // Biases
    double *hidden_layer_bias = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    double *output_layer_bias = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);

    // Weights
    double **hidden_weights = (double **)malloc(sizeof(double *) * INPUTS_NUMBER);
    for (int i = 0; i < INPUTS_NUMBER; i++)
    {
        hidden_weights[i] = (double *)malloc(sizeof(double) * HIDDEN_NODES_NUMBER);
    }

    double **output_weights = (double **)malloc(sizeof(double *) * HIDDEN_NODES_NUMBER);
    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++)
    {
        output_weights[i] = (double *)malloc(sizeof(double) * OUTPUTS_NUMBER);
    }

    // Initialize weights and biases
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

    // Load dataset
    iImage *images[MAX_IMAGES];
    int total_images = load_dataset("../../mydataset", images);
    if (total_images == 0)
    {
        printf("No images loaded.\n");
        return -1;
    }
    printf("Loaded %d images.\n", total_images);

    int number_of_epochs = 100; // Adjust based on your needs

    // Training loop
    for (int epoch = 0; epoch < number_of_epochs; epoch++)
    {
        // Shuffle images
        shuffle_images(images, total_images);

        for (int x = 0; x < total_images; x++)
        {
            // Flatten image pixels to input array
            double input_layer[INPUTS_NUMBER];
            int idx = 0;
            for (int i = 0; i < images[x]->height; i++)
            {
                for (int j = 0; j < images[x]->width; j++)
                {
                    Uint8 pixel_value = images[x]->pixels[i][j].r;    // Assuming binary image stored in red channel
                    input_layer[idx++] = pixel_value > 0 ? 1.0 : 0.0; // Binary values: 1 or 0
                }
            }

            // Forward pass
            // Compute activations for hidden layer
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
            {
                double activation = hidden_layer_bias[j];
                for (int k = 0; k < INPUTS_NUMBER; k++)
                {
                    activation += input_layer[k] * hidden_weights[k][j];
                }
                hidden_layer[j] = sigmoid(activation);
            }

            // Compute activations for output layer
            for (int j = 0; j < OUTPUTS_NUMBER; j++)
            {
                double activation = output_layer_bias[j];
                for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
                {
                    activation += hidden_layer[k] * output_weights[k][j];
                }
                output_layer[j] = activation; // No activation yet
            }

            // Apply softmax activation
            softmax(output_layer, OUTPUTS_NUMBER);

            // Expected output (one-hot encoding)
            double expected_output[OUTPUTS_NUMBER] = {0};
            expected_output[images[x]->label] = 1.0;

            // Backpropagation
            // Calculate output layer deltas
            double delta_output[OUTPUTS_NUMBER];
            for (int j = 0; j < OUTPUTS_NUMBER; j++)
            {
                delta_output[j] = output_layer[j] - expected_output[j];
            }

            // Calculate hidden layer deltas
            double delta_hidden[HIDDEN_NODES_NUMBER];
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
            {
                double error = 0.0;
                for (int k = 0; k < OUTPUTS_NUMBER; k++)
                {
                    error += delta_output[k] * output_weights[j][k];
                }
                delta_hidden[j] = error * d_sigmoid(hidden_layer[j]);
            }

            // Update weights and biases for output layer
            for (int j = 0; j < OUTPUTS_NUMBER; j++)
            {
                output_layer_bias[j] -= delta_output[j] * lr;
                for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
                {
                    output_weights[k][j] -= hidden_layer[k] * delta_output[j] * lr;
                }
            }

            // Update weights and biases for hidden layer
            for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
            {
                hidden_layer_bias[j] -= delta_hidden[j] * lr;
                for (int k = 0; k < INPUTS_NUMBER; k++)
                {
                    hidden_weights[k][j] -= input_layer[k] * delta_hidden[j] * lr;
                }
            }
        }

        printf("Epoch %d/%d completed.\n", epoch + 1, number_of_epochs);
    }

    while (1)
    {
        // Testing the neural network with a new image
        char test_image_path[256];
        printf("Enter the path to a test image (or 'exit' to quit): ");
        scanf("%s", test_image_path);

        if (strcmp(test_image_path, "exit") == 0)
        {
            break;
        }

        iImage *test_image = load_image(test_image_path, -1); // Label is not needed for testing
        if (test_image == NULL)
        {
            printf("Failed to load test image.\n");
            continue;
        }

        double test_input[INPUTS_NUMBER];
        int idx = 0;
        for (int i = 0; i < test_image->height; i++)
        {
            for (int j = 0; j < test_image->width; j++)
            {
                Uint8 pixel_value = test_image->pixels[i][j].r;  // Assuming binary image stored in red channel
                test_input[idx++] = pixel_value > 0 ? 1.0 : 0.0; // Binary values: 1 or 0
            }
        }

        // Compute activations for hidden layer
        for (int j = 0; j < HIDDEN_NODES_NUMBER; j++)
        {
            double activation = hidden_layer_bias[j];
            for (int k = 0; k < INPUTS_NUMBER; k++)
            {
                activation += test_input[k] * hidden_weights[k][j];
            }
            hidden_layer[j] = sigmoid(activation);
        }

        // Compute activations for output layer
        for (int j = 0; j < OUTPUTS_NUMBER; j++)
        {
            double activation = output_layer_bias[j];
            for (int k = 0; k < HIDDEN_NODES_NUMBER; k++)
            {
                activation += hidden_layer[k] * output_weights[k][j];
            }
            output_layer[j] = activation; // No activation yet
        }

        softmax(output_layer, OUTPUTS_NUMBER);

        int predicted_label = 0;
        double max_output = output_layer[0];
        for (int i = 1; i < OUTPUTS_NUMBER; i++)
        {
            if (output_layer[i] > max_output)
            {
                max_output = output_layer[i];
                predicted_label = i;
            }
        }

        printf("Predicted Letter: %c\n", 'A' + predicted_label);

        // Free test_image
        for (int y = 0; y < test_image->height; y++)
        {
            free(test_image->pixels[y]);
        }
        free(test_image->pixels);
        free(test_image->path);
        free(test_image);
    }

    // Free allocated memory
    for (int i = 0; i < total_images; i++)
    {
        for (int y = 0; y < images[i]->height; y++)
        {
            free(images[i]->pixels[y]);
        }
        free(images[i]->pixels);
        free(images[i]->path);
        free(images[i]);
    }

    // Free neural network memory
    free(hidden_layer);
    free(output_layer);
    free(hidden_layer_bias);
    free(output_layer_bias);

    for (int i = 0; i < INPUTS_NUMBER; i++)
    {
        free(hidden_weights[i]);
    }
    free(hidden_weights);

    for (int i = 0; i < HIDDEN_NODES_NUMBER; i++)
    {
        free(output_weights[i]);
    }
    free(output_weights);

    // Quit SDL2
    IMG_Quit();
    SDL_Quit();

    return 0;
}