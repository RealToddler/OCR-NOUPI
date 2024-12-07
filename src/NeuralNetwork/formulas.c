#include "formulas.h"
#include <math.h>
#include <stdlib.h>

// Activation functions
double sigmoid(double x) { return 1 / (1 + exp(-x)); }

double d_sigmoid(double x) { return x * (1 - x); }

// Softmax function
void softmax(double *output_layer, int size) {
  double max = output_layer[0];
  for (int i = 1; i < size; i++) {
    if (output_layer[i] > max)
      max = output_layer[i];
  }

  double sum = 0.0;
  for (int i = 0; i < size; i++) {
    output_layer[i] = exp(output_layer[i] - max);
    sum += output_layer[i];
  }

  for (int i = 0; i < size; i++) {
    output_layer[i] /= sum;
  }
}

// Weight initialization
double init_weights() {
  return ((double)rand() / RAND_MAX) * 2 - 1; // Random between -1 and 1
}