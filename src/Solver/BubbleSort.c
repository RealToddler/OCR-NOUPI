#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Split a line into tokens based on spaces
char **split_line(const char *line) {
  if (line == NULL) {
    return NULL;
  }

  int max_tokens = 1;
  for (const char *p = line; *p != '\0'; p++) {
    if (*p == ' ') {
      max_tokens++;
    }
  }

  char **tokens = malloc((max_tokens + 1) * sizeof(char *));
  if (tokens == NULL) {
    perror("malloc failed");
    return NULL;
  }

  char *line_copy = strdup(line);
  if (line_copy == NULL) {
    perror("strdup failed");
    free(tokens);
    return NULL;
  }

  int token_count = 0;
  char *token = strtok(line_copy, " ");
  while (token != NULL) {
    tokens[token_count++] = strdup(token);
    token = strtok(NULL, " ");
  }

  tokens[token_count] = NULL;

  free(line_copy);
  return tokens;
}

// Free memory allocated for an array of tokens
void free_tokens(char **tokens) {
  if (tokens == NULL)
    return;

  for (int i = 0; tokens[i] != NULL; i++) {
    free(tokens[i]);
  }
  free(tokens);
}

// Sort grid1 and grid2 based on x-coordinates using bubble sort
void bubbleSortGrids(char **grid1, char **grid2, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    char **splitted1 = split_line(grid1[i]);
    char *splitted2 = strdup(grid2[i]);

    for (int pass = 0; pass < cols - 1; pass++) {
      for (int j = 0; j < cols - pass - 1; j++) {
        int x1 = 0, y1 = 0;
        int x2 = 0, y2 = 0;
        sscanf(splitted1[j], "(%d,%d)", &x1, &y1);
        sscanf(splitted1[j + 1], "(%d,%d)", &x2, &y2);

        if (x1 > x2) {
          char *temp1 = splitted1[j];
          splitted1[j] = splitted1[j + 1];
          splitted1[j + 1] = temp1;

          char temp2 = splitted2[j];
          splitted2[j] = splitted2[j + 1];
          splitted2[j + 1] = temp2;
        }
      }
    }

    size_t size1 = 1;
    for (int k = 0; k < cols; k++) {
      size1 += strlen(splitted1[k]) + 1;
    }

    free(grid1[i]);
    grid1[i] = malloc(size1);
    if (!grid1[i]) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
    }

    grid1[i][0] = '\0';
    for (int k = 0; k < cols; k++) {
      strcat(grid1[i], splitted1[k]);
      if (k < cols - 1) {
        strcat(grid1[i], " ");
      }
    }

    free(grid2[i]);
    grid2[i] = strdup(splitted2);
    

    free_tokens(splitted1);
    free(splitted2);
  }
}