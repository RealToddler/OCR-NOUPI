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
} bBoundingBox;

typedef struct {
    bBoundingBox *boxes;
    int nb_boxes;
} bBoxSize;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} cColor;

void draw_line(iImage *img, int x0, int y0, int x1, int y1, cColor color);

void draw_quadrilateral(iImage *img, int xi, int yi, int xf, int yf, cColor color, int nb, int mod);

    void flood_fill(unsigned char **edge_map, int **label_map, int x, int y,
                    int height, int width, int label, bBoundingBox *box);
void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y,
                    cColor color);
void find_bounding_boxes(unsigned char **edge_map, unsigned int height,
                         unsigned int width, bBoundingBox **boxes,
                         int *num_boxes);
void merge_bounding_boxes(bBoundingBox *boxes, int *num_boxes, int xmargin,
                          int ymargin);

#endif // BOXES_H