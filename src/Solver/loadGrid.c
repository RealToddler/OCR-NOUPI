#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadGrid.h"

// Free the memory allocated for a gGrid structure
void free_grid(gGrid *grid) {
  if (grid) {
    for (int i = 0; i < grid->rows; i++) {
      free(grid->grid[i]);
    }
    free(grid->grid);
    free(grid);
  }
}

// Retrieve the value at a specific row and column in the grid
char *get_val(gGrid *coords, int row, int col) {
  char *line = strdup(coords->grid[row]);
  char *token = strtok(line, " ");
  int current_col = 0;

  while (token != NULL && current_col < col) {
    token = strtok(NULL, " ");
    current_col++;
  }

  char *result = token ? strdup(token) : NULL;
  free(line);
  return result;
}

// Load a grid from a file and return a gGrid struct
gGrid *load_grid(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }

  char line[1024];
  int rows = 0;
  int cols = 0;
  char **grid = NULL;

  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0';

    int is_coordinate = strchr(line, '(') != NULL;

    if (cols == 0) {
      if (is_coordinate) {
        char *temp = strdup(line);
        char *token = strtok(temp, " ");
        while (token) {
          cols++;
          token = strtok(NULL, " ");
        }
        free(temp);
      } else {
        cols = strlen(line);
      }
    }

    grid = realloc(grid, sizeof(char *) * (rows + 1));
    if (grid == NULL) {
      perror("Error reallocating memory for grid");
      fclose(file);
      exit(1);
    }

    if (is_coordinate) {
      grid[rows] = malloc(sizeof(char) * (strlen(line) + 1));
      if (grid[rows] == NULL) {
        perror("Error allocating memory for coordinate grid row");
        fclose(file);
        exit(1);
      }
      strcpy(grid[rows], line);
    } else {
      grid[rows] = malloc(sizeof(char) * (cols + 1));
      if (grid[rows] == NULL) {
        perror("Error allocating memory for letter grid row");
        fclose(file);
        exit(1);
      }
      strncpy(grid[rows], line, cols);
      grid[rows][cols] = '\0';
    }

    rows++;
  }

  fclose(file);

  gGrid *result = malloc(sizeof(gGrid));
  if (result == NULL) {
    perror("Error allocating memory for gGrid");
    exit(1);
  }
  result->grid = grid;
  result->rows = rows;
  result->cols = cols;

  return result;
}
