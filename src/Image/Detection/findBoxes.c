#include "findBoxes.h"
#include "Image/image.h"
#include "boxes.h"
#include "canny.h"
#include "refineImage.h"

#include <stdio.h>

// Filters and computes a bounding box enclosing valid boxes.
bBoundingBox compute_array(bBoundingBox *boxes, int numBoxes) {

  double wavrage = compute_median2(boxes, 1, numBoxes);
  double wavrage2 = compute_median2(boxes, 2, numBoxes);
  bBoundingBox *boxes_inter = sort(boxes, 1, numBoxes);

  int x_min = 2500;
  int x_max = 0;
  int y_min = 2500;
  int y_max = 0;

  for (int i = 0; i < numBoxes - 1; i++) {
    if (boxes_inter[i].height > (wavrage - (wavrage / 2)) &&
        boxes_inter[i].height < (wavrage + (wavrage / 3)) &&
        boxes_inter[i].width < (2 * (wavrage)) &&
        boxes_inter[i].width < (wavrage2 + (wavrage2 / 3))) {
      if (boxes_inter[i].max_x > x_max)
        x_max = boxes_inter[i].max_x;
      if (boxes_inter[i].max_y > y_max)
        y_max = boxes_inter[i].max_y;
      if (boxes_inter[i].min_x < x_min)
        x_min = boxes_inter[i].min_x;
      if (boxes_inter[i].min_y < y_min)
        y_min = boxes_inter[i].min_y;
    }
  }
  int h = (y_max - (y_min));
  int w = (x_max - (x_min));
  int surface = w * h;
  bBoundingBox res = {x_min, x_max, y_min, y_max, h, w, surface};

  return res;
}

// Finds boxes outside the main array with specific size criteria.
bBoxSize compute_word_list(bBoundingBox *boxes, int numBoxes) {

  bBoundingBox array = compute_array(boxes, numBoxes);
  bBoundingBox *res = malloc(numBoxes * sizeof(bBoundingBox));
  int index = 0;

  for (int i = 0; i < numBoxes - 1; i++) {
    if ((array.max_x < boxes[i].min_x || boxes[i].max_x < array.min_x ||
         array.max_y < boxes[i].min_y || boxes[i].max_y < array.min_y) &&
        boxes[i].height < boxes[i].width && boxes[i].surface < 30000) {
      res[index] = boxes[i];
      index++;
    }
  }
  res = realloc(res, index * sizeof(bBoundingBox));

  bBoxSize res2 = {res, index};
  return res2;
}
