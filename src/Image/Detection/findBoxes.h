#ifndef FINDBOXES_H
#define FINDBOXES_H

#include "../image.h"
#include "boxes.h"
#include "canny.h"

bBoundingBox compute_array(bBoundingBox *boxes, int numBoxes);

bBoxSize compute_word_list(bBoundingBox *boxes, int numBoxes);

#endif // FINDBOXES_H