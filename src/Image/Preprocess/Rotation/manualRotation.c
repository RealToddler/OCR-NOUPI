#include <math.h>
#include <stdlib.h>

#include "image.h"

#define PI 3.14159265

void rotation_matrix(double theta_deg, double R[2][2])
{
    double theta = theta_deg * PI / 180.0;
    R[0][0] = cos(theta);
    R[0][1] = -sin(theta);
    R[1][0] = sin(theta);
    R[1][1] = cos(theta);
}

void rotate_point(double x, double y, double center_x, double center_y, double R[2][2], double *new_x, double *new_y)
{
    double x_centered = x - center_x;
    double y_centered = y - center_y;

    *new_x = R[0][0] * x_centered + R[0][1] * y_centered + center_x;
    *new_y = R[1][0] * x_centered + R[1][1] * y_centered + center_x;
}

iImage *rotate_image(iImage *image, double angle_deg)
{
    unsigned int width = image->width;
    unsigned int height = image->height;
    double center_x = width / 2.0;
    double center_y = height / 2.0;

    double R[2][2];
    rotation_matrix(angle_deg, R);

    iImage *rotated_image = (iImage *)malloc(sizeof(iImage));
    rotated_image->width = width;
    rotated_image->height = height;
    rotated_image->pixels = (pPixel **)malloc(height * sizeof(pPixel *));
    for (unsigned int i = 0; i < height; i++)
    {
        rotated_image->pixels[i] = (pPixel *)malloc(width * sizeof(pPixel));
    }

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            rotated_image->pixels[y][x].r = 255;
            rotated_image->pixels[y][x].g = 255;
            rotated_image->pixels[y][x].b = 255;
        }
    }

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            double new_x, new_y;
            rotate_point(x, y, center_x, center_y, R, &new_x, &new_y);

            if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height)
            {
                rotated_image->pixels[(int)new_y][(int)new_x] = image->pixels[y][x];
            }
        }
    }

    return rotated_image;
}