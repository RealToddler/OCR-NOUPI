#ifndef ROTATION_H
#define ROTATION_H

#include "Image/image.h"

#define ROTATION_THRESHOLD 5.0

// Function to compute the 2D rotation matrix for a given angle
void rotation_matrix(double theta_deg, double R[2][2]);

// Function to rotate a point (x, y) around a center using a rotation matrix
void rotate_point(double x, double y, double center_x, double center_y,
                  double R[2][2], double *new_x, double *new_y);

// Function to compute the new dimensions of an image after rotation
void compute_new_dimensions(unsigned int width, unsigned int height,
                            double angle_deg, unsigned int *new_width,
                            unsigned int *new_height);

// Function to rotate an image by a specific angle
iImage *rotate_image(iImage *image, double angle_deg);

// Function to detect edges and their orientations in an image
void detect_edges(iImage *image, double **edges, double **angles);

// Function to find the dominant angle of edges in an image
double find_dominant_angle(double **edges, double **angles, unsigned int width,
                           unsigned int height);

// Function to determine the rotation angle needed to align the image
double determine_rotation_angle(iImage *image);

// Function to auto-rotate an image based on its content
char *rotate_image_auto(char *image);

char *rotate_original_auto(char *path);

#endif // ROTATION_H
