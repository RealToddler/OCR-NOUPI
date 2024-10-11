#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <dirent.h> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Image/image.h"

#define IMAGE_SIZE 32
#define INPUTS_NUMBER (IMAGE_SIZE * IMAGE_SIZE)
#define HIDDEN_NODES_NUMBER 24
#define OUTPUTS_NUMBER 26
#define MAX_IMAGES 15000

double sigmoid(double x);
double d_sigmoid(double x);
double init_weights();
int load_dataset(const char *dataset_path, iImage **images);
void shuffle_images(iImage **images, int n);

#endif // NEURALNETWORK_H
