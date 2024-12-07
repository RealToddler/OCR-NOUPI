#ifndef GRIDBUILDER_H
#define GRIDBUILDER_H

#define MAX_LINE_LENGTH 1024

#include "NeuralNetwork/neuralNetwork.h"

typedef struct {
  char *filename;
  int xcoords;
  int ycoords;
} FileEntry;

int is_empty_line(const char *line);
int extract_coordinates(const char *filename, int *x, int *y);
int compare_files(const void *a, const void *b);
FileEntry *process_files(const char *directory, int *file_count);
void reconstruct_grid(const char *directory, FileEntry *file_list,
                      int file_count, NeuralNetwork *nn);
void remove_empty_lines(const char *input_filename,
                        const char *output_filename);
int grid_builder();

#endif // GRIDBUILDER_H
