#include <dirent.h>
#include <stdio.h>
#include <string.h>

// This function search for the grid img, containing gridfile in its label
char *find_grid_file(const char *directory) {
  struct dirent *entry;
  DIR *dir = opendir(directory);

  if (dir == NULL) {
    perror("Error opening directory");
    return NULL;
  }

  char *grid_file = NULL;

  while ((entry = readdir(dir)) != NULL) {
    if (strstr(entry->d_name, "gridfile") != NULL) {
      grid_file = strdup(entry->d_name);
      break;
    }
  }

  closedir(dir);
  return grid_file;
}
