#ifndef EXTRACT_H
#define EXTRACT_H

#include "../image.h"
#include "boxes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_image(iImage *img, int index, cColor color);

#endif // EXTRACT_H