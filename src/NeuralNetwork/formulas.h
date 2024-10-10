#ifndef FORMULAS_H
#define FORMULAS_H

double sigmoid(double x);
double d_sigmoid(double x);
void softmax(double *output_layer, int size);
double init_weights();

#endif // FORMULAS_H
