#ifndef CANNYPARAMETERS_H
#define CANNYPARAMETERS_H

#include "../image.h"
#include "boxes.h"

void find_grid(bBoundingBox *boxes, int num_boxes, iImage *img);

void find_letters_in_word(bBoundingBox *boxes, int num_boxes, iImage *img);

void find_words_in_words_lists(bBoundingBox *boxes, int num_boxes, iImage *img);

void find_word_lists(bBoundingBox *boxes, int num_boxes, iImage *img);

void all(bBoundingBox *boxes, int num_boxes, iImage *img);

#endif // CANNYPARAMETER_H