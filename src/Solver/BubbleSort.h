#ifndef BUBBLESORT_H
#define BUBBLESORT_H

char **split_line(const char *line);
void free_tokens(char **tokens);
void bubbleSortGrids(char **grid1, char **grid2, int rows, int cols);

#endif // BUBBLESORT_H