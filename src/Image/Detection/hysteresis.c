#include "../image.h"

void hysteresis_recursive(unsigned char **edge_map, int y, int x, int height,
                          int width) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0)
                continue;
            int ny = y + dy;
            int nx = x + dx;
            if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                if (edge_map[ny][nx] == 1) {
                    edge_map[ny][nx] = 2;
                    hysteresis_recursive(edge_map, ny, nx, height, width);
                }
            }
        }
    }
}

void hysteresis_thresholding(iImage *img, float **edges, float low_thresh,
                             float high_thresh, unsigned char **edge_map) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            edge_map[y][x] = 0;
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            if (edges[y][x] >= high_thresh) {
                edge_map[y][x] = 2;
            } else if (edges[y][x] >= low_thresh) {
                edge_map[y][x] = 1;
            } else {
                edge_map[y][x] = 0;
            }
        }
    }

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            if (edge_map[y][x] == 2) {
                hysteresis_recursive(edge_map, y, x, img->height, img->width);
            }
        }
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            if (edge_map[y][x] == 1) {
                edge_map[y][x] = 0;
            } else if (edge_map[y][x] == 2) {
                edge_map[y][x] = 1;
            }
        }
    }
}