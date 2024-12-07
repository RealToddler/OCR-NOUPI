#ifndef OCR_H
#define OCR_H

#include <err.h>

#include "Image/Preprocess/ColorTreatment/binary.h"
#include "Image/Preprocess/ColorTreatment/grayscale.h"
#include "Image/Preprocess/ColorTreatment/invertColors.h"
#include "Image/Preprocess/ColorTreatment/isWhite.h"

#include "Image/Preprocess/NoiseReduction/contrasts.h"
#include "Image/Preprocess/NoiseReduction/gaussianBlur.h"
#include "Image/Preprocess/NoiseReduction/otsu.h"
#include "Image/Preprocess/NoiseReduction/sauvola.h"
#include "Image/Preprocess/NoiseReduction/smallGroupReduction.h"

#include "Image/crop.h"
#include "Image/image.h"
#include "Image/resize.h"

#include "Image/Detection/boxes.h"
#include "Image/Detection/canny.h"
#include "Image/Detection/cannyParameters.h"
#include "Image/Detection/extract.h"

#include "Image/Preprocess/Rotation/rotation.h"

#include "Solver/BubbleSort.h"
#include "Solver/checks.h"
#include "Solver/gridBuilder.h"
#include "Solver/loadGrid.h"
#include "Solver/solver.h"
#include "Solver/wordsBuilder.h"

#include "Files/findGrid.h"

iImage *pretreatment(const char *path, char **savePath);

int extraction(iImage *img);

int solve(const char *savePath, const char *grid_filename, char **solvedPath);

char *get_pretraited_path();

char *get_grid_file();

#endif