#include "../image.h"
#include "Image/crop.h"
#include "Image/resize.h"
#include "boxes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  This functions browser the entire image, looking for boxes of a certain color,
  in order to extracted its content
*/

void extract_image(iImage *img, cColor color) {

  int word_count = 0;
  int letter_count = 0;
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      pPixel *pixel = &img->pixels[y][x];
      if (pixel->r == color.r && pixel->g == color.g && pixel->b == color.b) {
        int w = 1;
        while ((x + w) < img->width) {
          pPixel *p = &img->pixels[y][x + w];
          if (p->r == color.r && p->g == color.g && p->b == color.b)
            w++;
          else
            break;
        }
        int h = 1;
        while ((y + h) < img->height) {
          pPixel *p = &img->pixels[y + h][x];
          if (p->r == color.r && p->g == color.g && p->b == color.b)
            h++;
          else
            break;
        }
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
            if (!(p->r == color.r && p->g == color.g && p->b == color.b)) {
              right_edge_valid = 0;
              break;
            }
          }
        }
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
            if (!(p->r == color.r && p->g == color.g && p->b == color.b)) {
              bottom_edge_valid = 0;
              break;
            }
          }
        }

        if (right_edge_valid && bottom_edge_valid) {
          int inner_width = w - 2;
          int inner_height = h - 2;

          char output_path[256];

          // red == grid
          if (color.r == 255 && color.g == 0 && color.b == 0) {
            snprintf(output_path, sizeof(output_path),
                     "resources/extracted/gridfile_%d_%d.png", x, y);
          }

          // blue == words
          else if (color.r == 0 && color.g == 0 && color.b == 255) {
            snprintf(output_path, sizeof(output_path),
                     "resources/extracted/words/word%d.png", word_count++);
          }

          // cyan = letters from grid
          else if (color.r == 43 && color.g == 255 && color.b == 255) {
            snprintf(output_path, sizeof(output_path),
                     "resources/extracted/grid_letters/%d_%d.png", x, y);
          }

          // pink = letters from word

          else if (color.r == 255 && color.g == 192 && color.b == 203) {
            snprintf(output_path, sizeof(output_path),
                     "resources/extracted/word_letters/%d.png", letter_count++);

          }

          else {

            snprintf(output_path, sizeof(output_path), "img_x%d_y%d.png", y, x);
          }

          if (inner_width > 0 && inner_height > 0) {
            iImage *new_img =
                create_image(inner_width, inner_height, output_path);

            if (!new_img) {
              fprintf(stderr, "Could not create new image\n");
              continue;
            }
            for (int yi_inner = 0; yi_inner < inner_height; yi_inner++) {
              for (int xi_inner = 0; xi_inner < inner_width; xi_inner++) {
                int src_x = x + 1 + xi_inner;
                int src_y = y + 1 + yi_inner;
                if (src_x >= img->width || src_y >= img->height) {
                  fprintf(stderr, "Index out of bounds when "
                                  "copying pixels.\n");
                  continue;
                }

                pPixel *src_pixel = &img->pixels[src_y][src_x];
                pPixel *dst_pixel = &new_img->pixels[yi_inner][xi_inner];
                dst_pixel->r = src_pixel->r;
                dst_pixel->g = src_pixel->g;
                dst_pixel->b = src_pixel->b;
              }
            }

            if ((color.r == 43 && color.g == 255 && color.b == 255) ||
                (color.r == 255 && color.g == 192 && color.b == 203)) {
              iImage *resized_img = resize_image(crop_image(new_img), 32, 32);
              if (resized_img) {
                save_image(resized_img, output_path);
                free_image(resized_img);
              }
            } else {
              save_image(new_img, output_path);
            }

            free_image(new_img);
          }

          for (int xi_border = x; xi_border < x + w; xi_border++) {
            if (xi_border < img->width && y < img->height) {
              pPixel *p = &img->pixels[y][xi_border];
              if (p->r == color.r && p->g == color.g && p->b == color.b) {
                p->r = 0;
                p->g = 255;
                p->b = 0;
              }
            }
            if (xi_border < img->width && y + h - 1 < img->height) {
              pPixel *p = &img->pixels[y + h - 1][xi_border];
              if (p->r == color.r && p->g == color.g && p->b == color.b) {
                p->r = 0;
                p->g = 255;
                p->b = 0;
              }
            }
          }

          for (int yi_border = y; yi_border < y + h; yi_border++) {
            if (x < img->width && yi_border < img->height) {
              pPixel *p = &img->pixels[yi_border][x];
              if (p->r == color.r && p->g == color.g && p->b == color.b) {
                p->r = 0;
                p->g = 255;
                p->b = 0;
              }
            }
            if (x + w - 1 < img->width && yi_border < img->height) {
              pPixel *p = &img->pixels[yi_border][x + w - 1];
              if (p->r == color.r && p->g == color.g && p->b == color.b) {
                p->r = 0;
                p->g = 255;
                p->b = 0;
              }
            }
          }
          x += w - 1;
        }
      }
    }
  }
}