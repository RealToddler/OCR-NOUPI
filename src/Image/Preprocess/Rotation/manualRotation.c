#include "Image/image.h"
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265

/*
    Compute the 2D rotation matrix for a given angle in degrees.
*/
void rotation_matrix(double theta_deg, double R[2][2]) {
    double theta = theta_deg * PI / 180.0;
    R[0][0] = cos(theta);
    R[0][1] = -sin(theta);
    R[1][0] = sin(theta);
    R[1][1] = cos(theta);
}

/*
    Rotate a point (x, y) around a center using a rotation matrix.
*/
void rotate_point(double x, double y, double center_x, double center_y,
                  double R[2][2], double *new_x, double *new_y) {
    double x_centered = x - center_x;
    double y_centered = y - center_y;

    *new_x = R[0][0] * x_centered + R[0][1] * y_centered + center_x;
    *new_y = R[1][0] * x_centered + R[1][1] * y_centered + center_y;
}

/*
    Compute the new bounding box dimensions for the rotated image.
*/
void compute_new_dimensions(unsigned int width, unsigned int height,
                            double angle_deg, unsigned int *new_width,
                            unsigned int *new_height) {
    double angle_rad = angle_deg * PI / 180.0;

    double cos_theta = fabs(cos(angle_rad));
    double sin_theta = fabs(sin(angle_rad));

    *new_width = (unsigned int)(width * cos_theta + height * sin_theta);
    *new_height = (unsigned int)(width * sin_theta + height * cos_theta);
}

/*
    Rotate an image by a given angle in degrees using bilinear interpolation and
    ensure no distortion. Background is set to white.
*/
iImage *rotate_image(iImage *image, double angle_deg) {
    unsigned int width = image->width;
    unsigned int height = image->height;

    unsigned int new_width, new_height;
    compute_new_dimensions(width, height, angle_deg, &new_width, &new_height);

    double center_x = width / 2.0;
    double center_y = height / 2.0;
    double new_center_x = new_width / 2.0;
    double new_center_y = new_height / 2.0;

    double R[2][2];
    rotation_matrix(-angle_deg, R);

    iImage *rotated_image = (iImage *)malloc(sizeof(iImage));
    rotated_image->width = new_width;
    rotated_image->height = new_height;
    rotated_image->pixels = (pPixel **)malloc(new_height * sizeof(pPixel *));
    for (unsigned int i = 0; i < new_height; i++) {
        rotated_image->pixels[i] = (pPixel *)malloc(new_width * sizeof(pPixel));
        for (unsigned int j = 0; j < new_width; j++) {
            rotated_image->pixels[i][j].r = 255;
            rotated_image->pixels[i][j].g = 255;
            rotated_image->pixels[i][j].b = 255;
        }
    }

    for (unsigned int y = 0; y < new_height; y++) {
        for (unsigned int x = 0; x < new_width; x++) {
            double src_x, src_y;

            rotate_point(x, y, new_center_x, new_center_y, R, &src_x, &src_y);

            src_x -= (new_center_x - center_x);
            src_y -= (new_center_y - center_y);

            if (src_x >= 0 && src_x < width - 1 && src_y >= 0 &&
                src_y < height - 1) {
                int x0 = (int)floor(src_x);
                int y0 = (int)floor(src_y);
                int x1 = x0 + 1;
                int y1 = y0 + 1;

                double dx = src_x - x0;
                double dy = src_y - y0;

                pPixel p00 = image->pixels[y0][x0];
                pPixel p01 = image->pixels[y0][x1];
                pPixel p10 = image->pixels[y1][x0];
                pPixel p11 = image->pixels[y1][x1];

                rotated_image->pixels[y][x].r =
                    (1 - dx) * (1 - dy) * p00.r + dx * (1 - dy) * p01.r +
                    (1 - dx) * dy * p10.r + dx * dy * p11.r;

                rotated_image->pixels[y][x].g =
                    (1 - dx) * (1 - dy) * p00.g + dx * (1 - dy) * p01.g +
                    (1 - dx) * dy * p10.g + dx * dy * p11.g;

                rotated_image->pixels[y][x].b =
                    (1 - dx) * (1 - dy) * p00.b + dx * (1 - dy) * p01.b +
                    (1 - dx) * dy * p10.b + dx * dy * p11.b;
            }
        }
    }

    return rotated_image;
}
