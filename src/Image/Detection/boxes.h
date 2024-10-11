#ifndef BOXES_H
#define BOXES_H

#include "../image.h"

typedef struct
{
    unsigned int min_x;
    unsigned int max_x;
    unsigned int min_y;
    unsigned int max_y;
    unsigned int height;
    unsigned int width;
    unsigned int surface;
} BoundingBox;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

void flood_fill(unsigned char **edge_map, int **label_map, unsigned int x, unsigned int y, unsigned int height, unsigned int width, int label, BoundingBox *box);
void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y, Color color);
void find_bounding_boxes(unsigned char **edge_map, unsigned int height, unsigned int width, BoundingBox **boxes, int *num_boxes);
void merge_bounding_boxes(BoundingBox *boxes, int *num_boxes);

#endif // BOXES_H