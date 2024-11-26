#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Image/Preprocess/ColorTreatment/binary.h"
#include "Image/Preprocess/ColorTreatment/grayscale.h"
#include "Image/Preprocess/ColorTreatment/invertColors.h"

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

#include "Image/Preprocess/Rotation/automaticRotation.h"
#include "Image/Preprocess/Rotation/manualRotation.h"

#include "NeuralNetwork/XNOR.h"

#include "Solver/checks.h"
#include "Solver/gridBuilder.h"
#include "Solver/loadGrid.h"
#include "Solver/solver.h"
#include "Solver/wordsBuilder.h"

#include "Files/findGrid.h"
#include "UserInterface/application.h"

/*
    Its name talks for itself
*/
int create_directory(const char *path) {
    char cmd[512];

    snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", path);

    int ret = system(cmd);

    if (ret != 0) {
        fprintf(stderr, "Error creating directory: %s\n", path);
        return -1;
    }

    return 0;
}

int main(int argc, char **agrv) {
  return open_app();
}