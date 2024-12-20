#ifndef HYSTERESIS_H
#define HYSTERESIS_H

void hysteresis_recursive(unsigned char **edge_map, int y, int x, int height,
                          int width);
void hysteresis_thresholding(iImage *img, float **edges, float low_thresh,
                             float high_thresh, unsigned char **edge_map);

#endif // HYSTERESIS_H