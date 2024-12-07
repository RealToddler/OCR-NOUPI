#include <ocr.h>
#include <time.h>

#define PRETREATED_PATH "resources/cache/pretraited.png"
#define SOLVED_PATH "resources/cache/solved.png"
#define GRID_DIRECTORY "resources/extracted/"

/*
  This function does everything that needs to be done to pretreat the image.
*/
iImage *pretreatment(const char *path, char **savePath) {
  iImage *img = load_image(path, -1);
  iImage *new_img = load_image(path, -1);

  if (img == NULL || new_img == NULL) {
    IMG_Quit();
    SDL_Quit();
    err(EXIT_FAILURE, "cant charge entry file");
  }

  grayscale(img);
  sauvola_threshold(img, 16);

  if (isWhite(img) == 1) {

    grayscale(new_img);
    sauvola_threshold_adaptative(new_img, 16);

    img = new_img;
  }

  apply_groups_reduction(img);

  *savePath = malloc(strlen(PRETREATED_PATH) + 1);
  if (*savePath == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    free_image(img);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  *savePath = malloc(strlen(PRETREATED_PATH) + 1);
  strcpy(*savePath, PRETREATED_PATH);

  save_image(img, *savePath);

  free(new_img);

  return img;
}

/*
  This function extracts everything that needs to be extracted.
  It also makes sure that temp files and folders are left created and left empty
  / deleted
*/
int extraction(iImage *img) {

  srand(time(NULL));

  sys_cmd("rm -f resources/extracted/grid_letters/*.png");
  sys_cmd("rm -f resources/extracted/text_data/*.txt");
  sys_cmd("rm -f resources/extracted/word_letters/*.png");
  sys_cmd("rm -f resources/extracted/words/*.png");
  sys_cmd("rm -f resources/extracted/*.png");
  sys_cmd("find resources/extracted/words_output -mindepth 1 -type d -exec "
          "rm -r {} +");

  cColor red = {255, 0, 0};
  cColor blue = {0, 0, 255};
  cColor cyan = {43, 255, 255};

  // detect and extract grid
  apply_canny(find_grid, img);
  extract_image(img, red);

  // detect and extract words
  apply_canny(find_word_lists, img);
  extract_image(img, blue);

  // save_image(img, "resources/test.png");

  char *grid_filename = find_grid_file(GRID_DIRECTORY);

  if (grid_filename == NULL) {
    fprintf(stderr, "no grid file found\n");
    free_image(img);
    sys_cmd("rm -f resources/extracted/grid_letters/*.png");
    sys_cmd("rm -f resources/extracted/text_data/*.txt");
    sys_cmd("rm -f resources/extracted/word_letters/*.png");
    sys_cmd("rm -f resources/extracted/words/*.png");
    sys_cmd("rm -f resources/extracted/*.png");

    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  char grid_path[1024];
  snprintf(grid_path, sizeof(grid_path), "%s%s", GRID_DIRECTORY, grid_filename);

  iImage *grid = load_image(grid_path, -1);
  if (grid == NULL) {
    fprintf(stderr, "Cant load the grid : %s\n", grid_path);
    free_image(img);
    free(grid_filename);
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  // detect and extract letters from grid
  apply_canny(all, grid);
  extract_image(grid, cyan);
  return 1;
}

/*
  This function solves the grid and circle the words on the grid
*/
int solve(const char *savePath, const char *grid_filename, char **solvedPath) {

  if (grid_builder() == 1) {
    err(EXIT_FAILURE, "an error occurend while building out the grid");
  }

  if (words_builder() == 1) {
    err(EXIT_FAILURE, "an error occurend while building out the words list");
  }

  // build txt data files
  gGrid *coords = load_grid("resources/extracted/txt_data/coordinates.txt");
  gGrid *letters = load_grid("resources/extracted/txt_data/letters.txt");
  if (coords == NULL || letters == NULL) {
    fprintf(stderr, "Cant load txt files\n");
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  bubbleSortGrids(coords->grid, letters->grid, coords->rows, coords->cols);

  int x_grid = 0, y_grid = 0;
  sscanf(grid_filename, "gridfile_%d_%d", &x_grid, &y_grid);

  iImage *res = load_image(savePath, -1);

  if (res == NULL) {
    fprintf(stderr, "cant charge entry file\n");
    IMG_Quit();
    SDL_Quit();
    free(res);
    return 0;
  }

  FILE *file = fopen("resources/extracted/txt_data/words.txt", "r");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open words.txt\n");
    return 0;
  }

  char word[1024];
  while (fgets(word, sizeof(word), file)) {
    word[strcspn(word, "\n")] = '\0';
    // printf("%s\n", word);

    cCoords wordCoords = solver(word, letters);

    tTuple t1 = wordCoords.t1;
    tTuple t2 = wordCoords.t2;

    int t1_x = 0, t1_y = 0, t2_x = 0, t2_y = 0;

    if ((t1.x != -1 && t2.x != -1) && (t1.y != -1 && t2.y != -1) &&
        strlen(word) > 2) {

      sscanf(get_val(coords, t1.y, t1.x), "(%d,%d)", &t1_x, &t1_y);
      sscanf(get_val(coords, t2.y, t2.x), "(%d,%d)", &t2_x, &t2_y);
      cColor color = {rand() % 255, rand() % 255, rand() % 255};

      if (t1.y == t2.y) // horizontal
      {
        if (t2_x - t1_x < 0) {
          int temp1 = t1_x;
          int temp2 = t1_y;
          t1_x = t2_x;
          t1_y = t2_y;
          t2_x = temp1;
          t2_y = temp2;
        }
        int last_l = (t2_x - t1_x) / ((strlen(word) - 1));

        draw_rectangle(res, t1_x + x_grid, t1_y + y_grid,
                       t2_x + last_l + x_grid, t2_y + y_grid + last_l * 0.8,
                       color, 3);
      } else if (t1.x == t2.x) { // vertical
        if (t2_y - t1_y < 0) {
          int temp1 = t1_x;
          int temp2 = t1_y;
          t1_x = t2_x;
          t1_y = t2_y;
          t2_x = temp1;
          t2_y = temp2;
        }
        int last_l = (t2_y - t1_y) / ((int)(strlen(word)));

        draw_rectangle(res, t1_x + x_grid - 10, t1_y + y_grid,
                       t2_x + x_grid + last_l, t2_y + y_grid + last_l, color,
                       3);
      } else {
        if (t2_y - t1_y < 0) {
          int temp1 = t1_x;
          int temp2 = t1_y;
          t1_x = t2_x;
          t1_y = t2_y;
          t2_x = temp1;
          t2_y = temp2;
        }
        if (t2_x < t1_x) {
          draw_diagonal(res, t1_x + x_grid, t1_y + y_grid, t2_x + x_grid,
                        t2_y + y_grid, color, strlen(word), 2, 3);
        } else {

          draw_diagonal(res, t1_x + x_grid, t1_y + y_grid, t2_x + x_grid,
                        t2_y + y_grid, color, strlen(word), 1, 3);
        }
      }
    }
  }

  fclose(file);
  save_image(res, SOLVED_PATH);

  *solvedPath = malloc(strlen(SOLVED_PATH) + 1);
  if (*solvedPath == NULL) {
    fprintf(stderr, "Memory allocation error for solvedPath\n");
    return 0;
  }
  strcpy(*solvedPath, SOLVED_PATH);

  free_grid(coords);
  free_grid(letters);

  IMG_Quit();
  SDL_Quit();

  return 1;
}

/*
  Its name talks for itself
*/
char *get_pretraited_path() {
  char *path = malloc(strlen(PRETREATED_PATH) + 1);
  if (path == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    return NULL;
  }
  strcpy(path, PRETREATED_PATH);
  return path;
}

/*
  Its name talks for itself
*/
char *get_grid_file() {
  char *gridfile = find_grid_file(GRID_DIRECTORY);
  char *path = malloc(strlen(gridfile) + 1);
  if (path == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    return NULL;
  }
  strcpy(path, gridfile);
  return path;
}