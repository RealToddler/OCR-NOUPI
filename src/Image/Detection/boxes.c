#include "boxes.h"
#include "../image.h"

void flood_fill(unsigned char **edge_map, int **label_map, unsigned int x,
                unsigned int y, unsigned int height, unsigned int width,
                int label, BoundingBox *box) {
    typedef struct {
        int x;
        int y;
    } Point;

    Point *stack = (Point *)malloc(height * width * sizeof(Point));
    int stack_size = 0;
    stack[stack_size].x = x;
    stack[stack_size].y = y;
    stack_size++;

    label_map[y][x] = label;

    while (stack_size > 0) {
        stack_size--;
        int cx = stack[stack_size].x;
        int cy = stack[stack_size].y;

        if (cx < box->min_x)
            box->min_x = cx;
        if (cx > box->max_x)
            box->max_x = cx;
        if (cy < box->min_y)
            box->min_y = cy;
        if (cy > box->max_y)
            box->max_y = cy;

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = cx + dx;
                int ny = cy + dy;
                if (dx == 0 && dy == 0)
                    continue;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (edge_map[ny][nx] == 1 && label_map[ny][nx] == 0) {
                        label_map[ny][nx] = label;
                        stack[stack_size].x = nx;
                        stack[stack_size].y = ny;
                        stack_size++;
                    }
                }
            }
        }
    }

    free(stack);
}

void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y,
                    Color color) {

    for (int x = min_x; x <= max_x; x++) {
        if (min_y >= 0 && min_y < img->height) {
            img->pixels[min_y][x].r = color.r;
            img->pixels[min_y][x].g = color.g;
            img->pixels[min_y][x].b = color.b;
        }
        if (max_y >= 0 && max_y < img->height) {
            img->pixels[max_y][x].r = color.r;
            img->pixels[max_y][x].g = color.g;
            img->pixels[max_y][x].b = color.b;
        }
    }

    for (int y = min_y; y <= max_y; y++) {
        if (min_x >= 0 && min_x < img->width) {
            img->pixels[y][min_x].r = color.r;
            img->pixels[y][min_x].g = color.g;
            img->pixels[y][min_x].b = color.b;
        }
        if (max_x >= 0 && max_x < img->width) {
            img->pixels[y][max_x].r = color.r;
            img->pixels[y][max_x].g = color.g;
            img->pixels[y][max_x].b = color.b;
        }
    }
}

void find_bounding_boxes(unsigned char **edge_map, unsigned int height,
                         unsigned int width, BoundingBox **boxes,
                         int *num_boxes) {
    int **label_map = (int **)malloc(height * sizeof(int *));
    for (unsigned int i = 0; i < height; i++) {
        label_map[i] = (int *)malloc(width * sizeof(int));
        for (unsigned int j = 0; j < width; j++) {
            label_map[i][j] = 0;
        }
    }

    int label = 1;
    *num_boxes = 0;
    BoundingBox *temp_boxes =
        (BoundingBox *)malloc(sizeof(BoundingBox) * height * width / 10);

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            if (edge_map[y][x] == 1 && label_map[y][x] == 0) {
                BoundingBox box;
                box.min_x = x;
                box.max_x = x;
                box.min_y = y;
                box.max_y = y;

                flood_fill(edge_map, label_map, x, y, height, width, label,
                           &box);

                box.height = box.max_y - box.min_y;
                box.width = box.max_x - box.min_x;
                box.surface = box.height * box.width;

                temp_boxes[*num_boxes] = box;
                (*num_boxes)++;
                label++;
            }
        }
    }

    *boxes = (BoundingBox *)malloc(sizeof(BoundingBox) * (*num_boxes));
    for (int i = 0; i < *num_boxes; i++) {
        (*boxes)[i] = temp_boxes[i];
    }

    free(temp_boxes);

    for (unsigned int i = 0; i < height; i++) {
        free(label_map[i]);
    }
    free(label_map);
}

void merge_bounding_boxes(BoundingBox *boxes, int *num_boxes) {
    int merged = 1;
    while (merged) {
        merged = 0;
        for (int i = 0; i < *num_boxes; i++) {
            for (int j = i + 1; j < *num_boxes; j++) {
                int overlap_x = (boxes[i].min_x <= boxes[j].max_x + 5) &&
                                (boxes[j].min_x <= boxes[i].max_x + 5);
                int overlap_y = (boxes[i].min_y <= boxes[j].max_y + 5) &&
                                (boxes[j].min_y <= boxes[i].max_y + 5);

                if (overlap_x && overlap_y) {
                    boxes[i].min_x = (boxes[i].min_x < boxes[j].min_x)
                                         ? boxes[i].min_x
                                         : boxes[j].min_x;
                    boxes[i].max_x = (boxes[i].max_x > boxes[j].max_x)
                                         ? boxes[i].max_x
                                         : boxes[j].max_x;
                    boxes[i].min_y = (boxes[i].min_y < boxes[j].min_y)
                                         ? boxes[i].min_y
                                         : boxes[j].min_y;
                    boxes[i].max_y = (boxes[i].max_y > boxes[j].max_y)
                                         ? boxes[i].max_y
                                         : boxes[j].max_y;

                    boxes[i].height = boxes[i].max_y - boxes[i].min_y;
                    boxes[i].width = boxes[i].max_x - boxes[i].min_x;
                    boxes[i].surface = boxes[i].height * boxes[i].width;

                    for (int k = j; k < *num_boxes - 1; k++) {
                        boxes[k] = boxes[k + 1];
                    }
                    (*num_boxes)--;
                    merged = 1;
                    break;
                }
            }
            if (merged)
                break;
        }
    }
}