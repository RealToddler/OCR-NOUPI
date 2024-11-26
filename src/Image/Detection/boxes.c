#include "boxes.h"
#include "../image.h"
#include <math.h>

void draw_line(iImage *img, int x0, int y0, int x1, int y1, cColor color) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        if (x0 >= 0 && x0 < img->width && y0 >= 0 && y0 < img->height) {
            img->pixels[y0][x0].r = color.r;
            img->pixels[y0][x0].g = color.g;
            img->pixels[y0][x0].b = color.b;
        }

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_quadrilateral(iImage *img, int xi, int yi, int xf, int yf, cColor color, int nb, int mod) {
    if(mod == 1){
    int size= (yf-yi)/((nb-1));
    draw_line(img, xi, yi, xi+size, yi, color);
    draw_line(img, xi, yi, xi, yi+size, color);
    draw_line(img, xi+size, yi, xf+size, yf, color);
    draw_line(img, xi, yi+size, xf, yf+size, color);
    draw_line(img, xf, yf+size, xf+size, yf+size, color);
    draw_line(img, xf+size, yf, xf+size, yf+size, color);}
    else{
        int size= (yf-yi)/(2*(nb-1));
        draw_line(img, xi, yi, xi+size, yi, color);
        draw_line(img, xi+size, yi, xi+size, yi+size, color);
        draw_line(img, xi+size, yi+size, xf+size, yf+size, color);
        draw_line(img, xi, yi, xf, yf, color);
        draw_line(img, xf, yf+size, xf+size, yf+size, color);
        draw_line(img, xf, yf, xf, yf+size, color);
    }

}

void flood_fill(unsigned char **edge_map, int **label_map, int x, int y,
                int height, int width, int label, bBoundingBox *box) {
    typedef struct {
        int x;
        int y;
    } pPoint;

    pPoint *stack = (pPoint *)malloc(height * width * sizeof(pPoint));
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
                    cColor color) {

    if ((max_x - min_x) * 2 < (max_y - min_y)) {
        min_x = min_x - (max_y - min_y) * 1 / 5;
        max_x = max_x + (max_y - min_y) * 1 / 5;
    }

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
                         unsigned int width, bBoundingBox **boxes,
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
    bBoundingBox *temp_boxes =
        (bBoundingBox *)malloc(sizeof(bBoundingBox) * height * width / 10);

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            if (edge_map[y][x] == 1 && label_map[y][x] == 0) {
                bBoundingBox box;
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

    *boxes = (bBoundingBox *)malloc(sizeof(bBoundingBox) * (*num_boxes));
    for (int i = 0; i < *num_boxes; i++) {
        (*boxes)[i] = temp_boxes[i];
    }

    free(temp_boxes);

    for (unsigned int i = 0; i < height; i++) {
        free(label_map[i]);
    }
    free(label_map);
}

void merge_bounding_boxes(bBoundingBox *boxes, int *num_boxes, int xmargin,
                          int ymargin) {
    int merged = 1;
    while (merged) {
        merged = 0;
        for (int i = 0; i < *num_boxes; i++) {
            for (int j = i + 1; j < *num_boxes; j++) {
                int overlap_x = (boxes[i].min_x <= boxes[j].max_x + xmargin) &&
                                (boxes[j].min_x <= boxes[i].max_x + xmargin);
                int overlap_y = (boxes[i].min_y <= boxes[j].max_y + ymargin) &&
                                (boxes[j].min_y <= boxes[i].max_y + ymargin);

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