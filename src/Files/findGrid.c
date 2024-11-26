#include <dirent.h> // For directory handling
#include <stdio.h>
#include <string.h>

// Function to find a file containing "grid" in its name in the specified
// directory
char *find_grid_file(const char *directory) {
    struct dirent *entry;
    DIR *dir = opendir(directory);

    if (dir == NULL) {
        perror("Error opening directory");
        return NULL;
    }

    char *grid_file = NULL;

    // Iterate through directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Check if the filename contains "grid"
        if (strstr(entry->d_name, "grid") != NULL) {
            grid_file = strdup(entry->d_name); // Duplicate the filename
            break; // Stop after finding the first match
        }
    }

    closedir(dir);
    return grid_file;
}
