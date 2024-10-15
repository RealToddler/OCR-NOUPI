#ifndef BOXES_H
#define BOXES_H

#include "../image.h"

typedef struct {
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int height;
    int width;
    int surface;
} BoundingBox;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

void flood_fill(unsigned char **edge_map, int **label_map, int x, int y,
                int height, int width, int label, BoundingBox *box);
void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y,
                    Color color);
void find_bounding_boxes(unsigned char **edge_map, unsigned int height,
                         unsigned int width, BoundingBox **boxes,
                         int *num_boxes);
void merge_bounding_boxes(BoundingBox *boxes, int *num_boxes);

#endif // BOXES_H