#include "refineImage.h"
#include "Image/image.h"

#include <stdio.h>

/*
  Thoses functions are utils functions in order to "make the data talk"
  Nothing very exotic, sorting, average, median, histogram and so on...
*/

int is_in_interval(long x, long y, long val) { return val >= x && val <= y; }

void erase(iImage *img, bBoundingBox boxe) {
  for (int y = boxe.min_y; y < boxe.max_y; y++) {
    for (int x = boxe.min_x; x < boxe.max_x; x++) {
      pPixel *pixel = &img->pixels[y][x];
      pixel->r = 0;
      pixel->g = 0;
      pixel->b = 0;
    }
  }
}

bBoundingBox *sort(bBoundingBox *boxes, int mod, int numBoxes) {
  bBoundingBox *res = boxes;
  bBoundingBox swap;

  if (mod == 1) // height
  {
    for (int i = 0; i < numBoxes; i++) {
      for (int j = 0; j < numBoxes - i - 1; j++) {
        if (boxes[j].height > boxes[j + 1].height) {
          swap = boxes[j];
          boxes[j] = boxes[j + 1];
          boxes[j + 1] = swap;
        }
      }
    }
  }

  if (mod == 2) // width
  {
    for (int i = 0; i < numBoxes; i++) {
      for (int j = 0; j < numBoxes - i - 1; j++) {
        if (boxes[j].width > boxes[j + 1].width) {
          swap = boxes[j];
          boxes[j] = boxes[j + 1];
          boxes[j + 1] = swap;
        }
      }
    }
  }

  if (mod == 3) // surface
  {
    for (int i = 0; i < numBoxes; i++) {
      for (int j = 0; j < numBoxes - i - 1; j++) {
        if (boxes[j].surface > boxes[j + 1].surface) {
          swap = boxes[j];
          boxes[j] = boxes[j + 1];
          boxes[j + 1] = swap;
        }
      }
    }
  }

  return res;
}

int *compute_histogram(bBoundingBox *sortedBoxes, int mod, int numBoxes) {
  int max;

  if (mod == 1) {
    max = sortedBoxes[numBoxes - 1].height;
  } else if (mod == 2) {
    max = sortedBoxes[numBoxes - 1].width;
  } else {
    max = sortedBoxes[numBoxes - 1].surface;
  }

  // we wont take 0 in consideration. Therefore will count
  // from 1 to max (included)

  // printf("%d\n", max);

  int *histogram = calloc(max + 1, sizeof(int));

  for (int i = 0; i < numBoxes; i++) {
    if (mod == 1) {
      if (sortedBoxes[i].height <= max) {
        histogram[sortedBoxes[i].height]++;
      }
    } else if (mod == 2) {
      if (sortedBoxes[i].width <= max) {
        histogram[sortedBoxes[i].width]++;
      }
    } else {
      if (sortedBoxes[i].surface <= max) {
        histogram[sortedBoxes[i].surface]++;
      }
    }
  }

  /*
  for (int i = 0; i < max + 1; i++)
  {
      printf("val = %d : %d occurences\n", i, histogram[i]);
  }
  */

  return histogram;
}

int compute_median(int *histogram, int size) {
  int total_count = 0;
  for (int i = 0; i < size; i++) {
    total_count += histogram[i];
  }

  int midpoint = total_count / 2;
  int cumulative_count = 0;
  int median_value = 0;
  for (int i = 0; i < size; i++) {
    cumulative_count += histogram[i];
    if (cumulative_count > midpoint) {
      if (total_count % 2 == 0 && cumulative_count - histogram[i] == midpoint) {
        int next_value = i + 1;
        while (next_value < size && histogram[next_value] == 0) {
          next_value++;
        }
        median_value = (i + next_value) / 2.0;
      } else {
        median_value = i;
      }
      break;
    }
  }

  return median_value;
}

double compute_average(bBoundingBox *boxes, int mod, int numBoxes) {

  double average = 0;
  for (int i = 0; i < numBoxes - 1; i++) {
    if (mod == 1) {
      average += boxes[i].height;
    } else if (mod == 2) {
      average += boxes[i].width;
    } else {
      average += boxes[i].surface;
    }
  }

  return average / numBoxes;
}

double compute_median2(bBoundingBox *boxes, int mod, int numBoxes) {

  double average = 0;
  for (int i = 15; i < numBoxes - 25; i++) {
    if (mod == 1) {
      average += boxes[i].height;
    } else if (mod == 2) {
      average += boxes[i].width;
    } else {
      average += boxes[i].surface;
    }
  }

  return average / (numBoxes - 30);
}