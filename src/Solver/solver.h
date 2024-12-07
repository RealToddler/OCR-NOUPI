#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checks.h"
#include "loadGrid.h"

cCoords solver(char *word, gGrid *grid);

#endif // SOLVER_H