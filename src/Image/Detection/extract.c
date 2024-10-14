#include "../image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_image(iImage *img) {
    int rect_counter = 0;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {

            // Vérifier si le pixel est rouge
            pPixel *pixel = &img->pixels[y][x];
            if (pixel->r == 255 && pixel->g == 0 && pixel->b == 0) {

                // Commencer à traiter le rectangle à partir de (x, y)

                // Trouver la largeur du rectangle
                int w = 1;
                while ((x + w) < img->width) {
                    pPixel *p = &img->pixels[y][x + w];
                    if (p->r == 255 && p->g == 0 && p->b == 0)
                        w++;
                    else
                        break;
                }

                // Trouver la hauteur du rectangle
                int h = 1;
                while ((y + h) < img->height) {
                    pPixel *p = &img->pixels[y + h][x];
                    if (p->r == 255 && p->g == 0 && p->b == 0)
                        h++;
                    else
                        break;
                }

                // Vérifier le bord droit
                int right_edge_valid = 1;
                int xi = x + w - 1;
                if (xi >= img->width) {
                    right_edge_valid = 0;
                } else {
                    for (int i = 0; i < h; i++) {
                        int yi = y + i;
                        if (yi >= img->height) {
                            right_edge_valid = 0;
                            break;
                        }
                        pPixel *p = &img->pixels[yi][xi];
                        if (!(p->r == 255 && p->g == 0 && p->b == 0)) {
                            right_edge_valid = 0;
                            break;
                        }
                    }
                }

                // Vérifier le bord inférieur
                int bottom_edge_valid = 1;
                int yi = y + h - 1;
                if (yi >= img->height) {
                    bottom_edge_valid = 0;
                } else {
                    for (int i = 0; i < w; i++) {
                        int xi = x + i;
                        if (xi >= img->width) {
                            bottom_edge_valid = 0;
                            break;
                        }
                        pPixel *p = &img->pixels[yi][xi];
                        if (!(p->r == 255 && p->g == 0 && p->b == 0)) {
                            bottom_edge_valid = 0;
                            break;
                        }
                    }
                }

                if (right_edge_valid && bottom_edge_valid) {
                    // Extraire la zone à l'intérieur du rectangle
                    int inner_width = w - 2;
                    int inner_height = h - 2;

                    char output_path[256];
                    snprintf(output_path, sizeof(output_path),
                             "temp/zrectangle_%d.png", ++rect_counter);

                    if (inner_width > 0 && inner_height > 0) {
                        iImage *new_img = create_image(
                            inner_width, inner_height, output_path);
                        if (!new_img) {
                            fprintf(stderr, "Could not create new image\n");
                            continue;
                        }

                        // Copier les pixels intérieurs
                        for (int yi_inner = 0; yi_inner < inner_height;
                             yi_inner++) {
                            for (int xi_inner = 0; xi_inner < inner_width;
                                 xi_inner++) {
                                int src_x = x + 1 + xi_inner;
                                int src_y = y + 1 + yi_inner;

                                // Vérification des limites
                                if (src_x >= img->width ||
                                    src_y >= img->height) {
                                    fprintf(stderr, "Index out of bounds when "
                                                    "copying pixels.\n");
                                    continue;
                                }

                                pPixel *src_pixel = &img->pixels[src_y][src_x];
                                pPixel *dst_pixel =
                                    &new_img->pixels[yi_inner][xi_inner];
                                dst_pixel->r = src_pixel->r;
                                dst_pixel->g = src_pixel->g;
                                dst_pixel->b = src_pixel->b;
                            }
                        }

                        // Enregistrer la nouvelle image
                        save_image(new_img, output_path);

                        // Libérer la nouvelle image
                        free_image(new_img);
                    }

                    // Transformer les bordures rouges en vert dans l'image
                    // originale

                    // Changer les bords supérieur et inférieur
                    for (int xi_border = x; xi_border < x + w; xi_border++) {
                        // Bord supérieur
                        if (xi_border < img->width && y < img->height) {
                            pPixel *p = &img->pixels[y][xi_border];
                            if (p->r == 255 && p->g == 0 && p->b == 0) {
                                p->r = 0;
                                p->g = 255;
                                p->b = 0;
                            }
                        }
                        // Bord inférieur
                        if (xi_border < img->width && y + h - 1 < img->height) {
                            pPixel *p = &img->pixels[y + h - 1][xi_border];
                            if (p->r == 255 && p->g == 0 && p->b == 0) {
                                p->r = 0;
                                p->g = 255;
                                p->b = 0;
                            }
                        }
                    }

                    // Changer les bords gauche et droit
                    for (int yi_border = y; yi_border < y + h; yi_border++) {
                        // Bord gauche
                        if (x < img->width && yi_border < img->height) {
                            pPixel *p = &img->pixels[yi_border][x];
                            if (p->r == 255 && p->g == 0 && p->b == 0) {
                                p->r = 0;
                                p->g = 255;
                                p->b = 0;
                            }
                        }
                        // Bord droit
                        if (x + w - 1 < img->width && yi_border < img->height) {
                            pPixel *p = &img->pixels[yi_border][x + w - 1];
                            if (p->r == 255 && p->g == 0 && p->b == 0) {
                                p->r = 0;
                                p->g = 255;
                                p->b = 0;
                            }
                        }
                    }

                    // Sauter le rectangle traité
                    x += w - 1;
                }
            }
        }
    }
}