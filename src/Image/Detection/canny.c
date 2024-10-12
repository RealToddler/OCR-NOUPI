#include "../image.h"
#include "boxes.h"
#include "canny.h"
#include "hysteresis.h"
#include "refineImage.h"

#define PI 3.14
#define MAX_SURFACE 1000000 // 1000 * 1000

// for the grid detection, we should stock in a heap every boxes with a surface between  200px et 350/400px
// then find the boxes with the highest x, lowest x, highest y and lowest y then draw a rectangle

// we may also implement hough transformation to get the grid when its draw

void calculate_gradients(iImage *img, float **gradient_magnitude, float **gradient_direction)
{
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};
    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            int gx_r = 0, gy_r = 0;
            int gx_g = 0, gy_g = 0;
            int gx_b = 0, gy_b = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    gx_r += img->pixels[y + ky][x + kx].r * Gx[ky + 1][kx + 1];
                    gy_r += img->pixels[y + ky][x + kx].r * Gy[ky + 1][kx + 1];

                    gx_g += img->pixels[y + ky][x + kx].g * Gx[ky + 1][kx + 1];
                    gy_g += img->pixels[y + ky][x + kx].g * Gy[ky + 1][kx + 1];

                    gx_b += img->pixels[y + ky][x + kx].b * Gx[ky + 1][kx + 1];
                    gy_b += img->pixels[y + ky][x + kx].b * Gy[ky + 1][kx + 1];
                }
            }

            float grad_r = sqrt(gx_r * gx_r + gy_r * gy_r);
            float grad_g = sqrt(gx_g * gx_g + gy_g * gy_g);
            float grad_b = sqrt(gx_b * gx_b + gy_b * gy_b);

            gradient_magnitude[y][x] = (grad_r + grad_g + grad_b) / 3.0;

            gradient_direction[y][x] = atan2(gy_r + gy_g + gy_b, gx_r + gx_g + gx_b) * 180 / PI;
        }
    }
}


void non_max_suppression(iImage *img, float **gradient_magnitude, float **gradient_direction, float **edges)
{
    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            float direction = gradient_direction[y][x];
            float magnitude = gradient_magnitude[y][x];
            float mag1, mag2;

            if ((direction >= -22.5 && direction <= 22.5) || (direction >= 157.5 || direction <= -157.5))
            {
                mag1 = gradient_magnitude[y][x - 1];
                mag2 = gradient_magnitude[y][x + 1];
            }
            else if ((direction > 22.5 && direction <= 67.5) || (direction < -112.5 && direction >= -157.5))
            {
                mag1 = gradient_magnitude[y - 1][x + 1];
                mag2 = gradient_magnitude[y + 1][x - 1];
            }
            else if ((direction > 67.5 && direction <= 112.5) || (direction < -67.5 && direction >= -112.5))
            {
                mag1 = gradient_magnitude[y - 1][x];
                mag2 = gradient_magnitude[y + 1][x];
            }
            else
            {
                mag1 = gradient_magnitude[y - 1][x - 1];
                mag2 = gradient_magnitude[y + 1][x + 1];
            }

            if (magnitude >= mag1 && magnitude >= mag2)
            {
                edges[y][x] = magnitude;
            }
            else
            {
                edges[y][x] = 0;
            }
        }
    }
}


void dilate(unsigned char **input, unsigned char **output, unsigned int height, unsigned int width)
{
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            output[y][x] = 0;
        }
    }

    for (unsigned int y = 1; y < height - 1; y++)
    {
        for (unsigned int x = 1; x < width - 1; x++)
        {
            if (input[y][x] == 1)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        output[y + dy][x + dx] = 1;
                    }
                }
            }
        }
    }
}

void apply_canny(iImage *img)
{
    float **gradient_magnitude = (float **)malloc(img->height * sizeof(float *));
    float **gradient_direction = (float **)malloc(img->height * sizeof(float *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        gradient_magnitude[i] = (float *)malloc(img->width * sizeof(float));
        gradient_direction[i] = (float *)malloc(img->width * sizeof(float));
    }
    calculate_gradients(img, gradient_magnitude, gradient_direction);

    float **edges = (float **)malloc(img->height * sizeof(float *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        edges[i] = (float *)malloc(img->width * sizeof(float));
    }
    non_max_suppression(img, gradient_magnitude, gradient_direction, edges);

    unsigned char **edge_map = (unsigned char **)malloc(img->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        edge_map[i] = (unsigned char *)malloc(img->width * sizeof(unsigned char));
    }
    float low_thresh = 20.0;
    float high_thresh = 50.0;
    hysteresis_thresholding(img, edges, low_thresh, high_thresh, edge_map);

    unsigned char **dilated_edge_map = (unsigned char **)malloc(img->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        dilated_edge_map[i] = (unsigned char *)malloc(img->width * sizeof(unsigned char));
    }
    dilate(edge_map, dilated_edge_map, img->height, img->width);

    BoundingBox *boxes;
    int num_boxes;
    find_bounding_boxes(dilated_edge_map, img->height, img->width, &boxes, &num_boxes);

    // merge_bounding_boxes(boxes, &num_boxes);
    Color red = {255, 0, 0};

    BoundingBox *sorted_by_height_boxes = sort(boxes, 1, num_boxes);
    int *histogram_by_height = compute_histogram(sorted_by_height_boxes, 1, num_boxes);
    double height_average = compute_average(sorted_by_height_boxes, 1, num_boxes);

    BoundingBox *sorted_by_width_boxes = sort(boxes, 2, num_boxes);
    int *histogram_by_width = compute_histogram(sorted_by_width_boxes, 2, num_boxes);
    double width_average = compute_average(sorted_by_height_boxes, 1, num_boxes);

    BoundingBox *sorted_by_surface_boxes = sort(boxes, 3, num_boxes);
    int *histogram_by_surface = compute_histogram(sorted_by_surface_boxes, 3, num_boxes);
    double surface_average = compute_average(sorted_by_height_boxes, 1, num_boxes);

    printf("%lf\n", compute_average(boxes, 1, num_boxes));
    for (int i = 0; i < num_boxes; i++)
    {
        if ((is_in_interval(height_average - 15, height_average + 15, boxes[i].height) || is_in_interval(width_average-15, width_average + 15, boxes[i].width)) && boxes[i].height + 15 >= boxes[i].width)
        {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x, boxes[i].max_y, red);
        }
        
        // printf("%d\n", boxes[i].surface);
        if (boxes[i].height < 5 || boxes[i].width < 5)
        {
            erase(img, boxes[i]);
        } else {
            // draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x, boxes[i].max_y, red);
        }
    }
    for (unsigned int i = 0; i < img->height; i++)
    {
        free(gradient_magnitude[i]);
        free(gradient_direction[i]);
        free(edges[i]);
        free(edge_map[i]);
        free(dilated_edge_map[i]);
    }
    free(gradient_magnitude);
    free(gradient_direction);
    free(edges);
    free(edge_map);
    free(dilated_edge_map);
    free(boxes);
}