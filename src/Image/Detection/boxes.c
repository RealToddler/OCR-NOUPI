#include "boxes.h"
#include "Image/image.h"
#include <math.h>

// Draws a line between two points (x0, y0) and (x1, y1) on an image with a
// specified color and thickness.
void draw_line(iImage *img, int x0, int y0, int x1, int y1, cColor color,
               int thickness) {
  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;
  int e2;

  int half_th = thickness / 2;

  while (1) {
    int start_x = x0 - half_th;
    int end_x = x0 + half_th;
    int start_y = y0 - half_th;
    int end_y = y0 + half_th;

    if (start_x < 0)
      start_x = 0;
    if (end_x >= img->width)
      end_x = img->width - 1;
    if (start_y < 0)
      start_y = 0;
    if (end_y >= img->height)
      end_y = img->height - 1;

    for (int py = start_y; py <= end_y; py++) {
      for (int px = start_x; px <= end_x; px++) {
        img->pixels[py][px].r = color.r;
        img->pixels[py][px].g = color.g;
        img->pixels[py][px].b = color.b;
      }
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

// Draws a diagonal pattern on the image, either solid or dashed, with the
// specified color and thickness.
void draw_diagonal(iImage *img, int xi, int yi, int xf, int yf, cColor color,
                   int nb, int mod, int thickness) {
  if (xi < 0) {
    xi = 0;
  }
  if (xf > img->width - 1) {
    xf = img->width - 1;
  }
  if (yi < 0) {
    yi = 0;
  }
  if (yf > img->height - 1) {
    yf = img->height - 1;
  }
  if (mod == 1) {
    int size = (yf - yi) / ((nb - 1)) * 0.85;
    draw_line(img, xi, yi, xi + size, yi, color, thickness);
    draw_line(img, xi, yi, xi, yi + size, color, thickness);
    draw_line(img, xi + size, yi, xf + size, yf, color, thickness);
    draw_line(img, xi, yi + size, xf, yf + size, color, thickness);
    draw_line(img, xf, yf + size, xf + size, yf + size, color, thickness);
    draw_line(img, xf + size, yf, xf + size, yf + size, color, thickness);
  } else {
    int size = (yf - yi) / (2 * (nb - 1)) * 1.2f;
    draw_line(img, xi, yi, xi + size, yi, color, thickness);
    draw_line(img, xi + size, yi, xi + size, yi + size, color, thickness);
    draw_line(img, xi + size, yi + size, xf + size, yf + size, color,
              thickness);
    draw_line(img, xi, yi, xf, yf, color, thickness);
    draw_line(img, xf, yf + size, xf + size, yf + size, color, thickness);
    draw_line(img, xf, yf, xf, yf + size, color, thickness);
  }
}

// Fills a connected region on the edge map with a specific label, updating
// bounding box coordinates.
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

// Draws a rectangle on an image using the specified bounding box coordinates,
// color, and thickness.
void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y,
                    cColor color, int thickness) {
  if (min_x < 0) {
    min_x = 0;
  }
  if (max_x > img->width - 1) {
    max_x = img->width - 1;
  }
  if (min_y < 0) {
    min_y = 0;
  }
  if (max_y > img->height - 1) {
    max_y = img->height - 1;
  }

  draw_line(img, min_x, min_y, max_x, min_y, color, thickness);
  draw_line(img, max_x, min_y, max_x, max_y, color, thickness);
  draw_line(img, max_x, max_y, min_x, max_y, color, thickness);
  draw_line(img, min_x, max_y, min_x, min_y, color, thickness);
}

// Identifies bounding boxes from an edge map by labeling connected regions and
// calculating their boundaries.
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

        flood_fill(edge_map, label_map, x, y, height, width, label, &box);

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

// Merges overlapping or adjacent bounding boxes based on specified margins,
// reducing the total count of boxes.
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
          boxes[i].min_x = (boxes[i].min_x < boxes[j].min_x) ? boxes[i].min_x
                                                             : boxes[j].min_x;
          boxes[i].max_x = (boxes[i].max_x > boxes[j].max_x) ? boxes[i].max_x
                                                             : boxes[j].max_x;
          boxes[i].min_y = (boxes[i].min_y < boxes[j].min_y) ? boxes[i].min_y
                                                             : boxes[j].min_y;
          boxes[i].max_y = (boxes[i].max_y > boxes[j].max_y) ? boxes[i].max_y
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
