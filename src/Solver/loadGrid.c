#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadGrid.h"

void free_grid(gGrid *grid) {
    if (grid) {
        for (int i = 0; i < grid->rows; i++) {
            free(grid->grid[i]);
        }
        free(grid->grid);
        free(grid);
    }
}

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

/*
    This function returns the grid loaded from a .txt file.
    It supports both coordinate grids (e.g., "(0,0) (52,0)") and letter grids
   (e.g., "MSWATERMELON").
 */
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
        line[strcspn(line, "\n")] = '\0'; // Remove the newline character

        // Determine if the line is a coordinate grid or a letter grid
        int is_coordinate = strchr(line, '(') !=
                            NULL; // Check for '(' to identify coordinate grids

        // Count columns only for the first line
        if (cols == 0) {
            if (is_coordinate) {
                // Count coordinate pairs
                char *temp = strdup(line);
                char *token = strtok(temp, " ");
                while (token) {
                    cols++;
                    token = strtok(NULL, " ");
                }
                free(temp);
            } else {
                // Count characters for letter grids
                cols = strlen(line);
            }
        }

        // Allocate memory for the grid
        grid = realloc(grid, sizeof(char *) * (rows + 1));
        if (grid == NULL) {
            perror("Error reallocating memory for grid");
            fclose(file);
            exit(1);
        }

        if (is_coordinate) {
            // Allocate memory for the coordinate row
            grid[rows] = malloc(sizeof(char) * (strlen(line) + 1));
            if (grid[rows] == NULL) {
                perror("Error allocating memory for coordinate grid row");
                fclose(file);
                exit(1);
            }
            strcpy(grid[rows], line);
        } else {
            // Allocate memory for the letter row
            grid[rows] =
                malloc(sizeof(char) * (cols + 1)); // +1 for null terminator
            if (grid[rows] == NULL) {
                perror("Error allocating memory for letter grid row");
                fclose(file);
                exit(1);
            }
            strncpy(grid[rows], line, cols);
            grid[rows][cols] = '\0'; // Null-terminate the string
        }

        rows++;
    }

    fclose(file);

    // Create and populate the gGrid structure
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
