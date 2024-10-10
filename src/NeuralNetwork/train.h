#ifndef TRAIN_H
#define TRAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "../Image/image.h"

int load_dataset(const char *dataset_path, iImage **images);
void shuffle_images(iImage **images, int n);
void apply_training(iImage *images[MAX_IMAGES], int total_images);

#endif // TRAIN_H
