#ifndef LETTERS_H
#define LETTERS_H

#include "../../image.h"

typedef struct
{
    int min_x;
    int max_x;
    int min_y;
    int max_y;
} BoundingBox;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

void calculate_gradients(iImage *img, float **gradient_magnitude, float **gradient_direction);
void non_max_suppression(iImage *img, float **gradient_magnitude, float **gradient_direction, float **edges);
void hysteresis_thresholding(iImage *img, float **edges, float low_thresh, float high_thresh, unsigned char **edge_map);
void dilate(unsigned char **input, unsigned char **output, unsigned int height, unsigned int width);
void find_bounding_boxes(unsigned char **edge_map, unsigned int height, unsigned int width, BoundingBox **boxes, int *num_boxes);
void merge_bounding_boxes(BoundingBox *boxes, int *num_boxes);
void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y, Color color);
void apply_canny(iImage *img);

#endif // LETTERS_H
