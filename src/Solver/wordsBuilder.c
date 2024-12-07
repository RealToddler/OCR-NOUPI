#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "wordsBuilder.h"

#include "Image/Detection/canny.h"
#include "Image/Detection/cannyParameters.h"
#include "Image/Detection/detectLetters.h"
#include "Image/Detection/extract.h"
#include "Image/crop.h"
#include "Image/image.h"
#include "Image/resize.h"

#include "NeuralNetwork/neuralNetwork.h"
#include "SysCommand/syscmd.h"

int extract_number(const char *fname) {
  while (*fname && !isdigit(*fname)) {
    fname++;
  }
  return *fname ? atoi(fname) : 0;
}

// Compare filenames based on embedded numbers
int compare_filenames_bis(const void *a, const void *b) {
  const char *fname1 = *(const char **)a;
  const char *fname2 = *(const char **)b;

  int num1 = extract_number(fname1);
  int num2 = extract_number(fname2);

  if (num1 != num2) {
    return num1 - num2;
  }

  return strcmp(fname1, fname2);
}

// Recognize a letter from an image using a neural network
char recognize_letter(const char *letter_full_path, NeuralNetwork *nn) {
  iImage *letter_image = load_image(letter_full_path, -1);
  if (letter_image == NULL) {
    err(EXIT_FAILURE, "couldn't load image");
  }

  double input[INPUTS_NUMBER];
  int idx = 0;
  for (int y = 0; y < letter_image->height; y++) {
    for (int x = 0; x < letter_image->width; x++) {
      Uint8 pixel_value = letter_image->pixels[y][x].r;
      input[idx++] = pixel_value > 0 ? 1.0 : 0.0;
    }
  }

  forward_pass(nn, input);

  int predicted_label = 0;
  double max_output = nn->output_layer[0];
  for (int j = 1; j < OUTPUTS_NUMBER; j++) {
    if (nn->output_layer[j] > max_output) {
      max_output = nn->output_layer[j];
      predicted_label = j;
    }
  }

  char letter = 'A' + predicted_label;

  free_image(letter_image);
  return letter;
}

// Process all letter images in a directory and build the word
void process_letters_in_word(NeuralNetwork *nn, FILE *words_file) {
  const char *letters_directory = "resources/extracted/word_letters";
  const char *output_directory = "resources/extracted/words_output";

  mkdir(output_directory, 0777);

  DIR *letters_dir = opendir(letters_directory);
  if (!letters_dir) {
    err(EXIT_FAILURE, "Couldn't open directory %s", letters_directory);
  }

  char **letter_filenames = NULL;
  int letter_file_count = 0;

  struct dirent *letter_entry;
  while ((letter_entry = readdir(letters_dir)) != NULL) {
    if (strstr(letter_entry->d_name, ".png")) {
      char *filename = strdup(letter_entry->d_name);
      if (!filename) {
        err(EXIT_FAILURE, "strdup error");
      }

      char **temp_list =
          realloc(letter_filenames, (letter_file_count + 1) * sizeof(char *));
      if (!temp_list) {
        err(EXIT_FAILURE, "realloc error");
      }
      letter_filenames = temp_list;
      letter_filenames[letter_file_count++] = filename;
    }
  }
  closedir(letters_dir);

  qsort(letter_filenames, letter_file_count, sizeof(char *),
        compare_filenames_bis);

  char word_buffer[MAX_LINE_LENGTH];
  int word_buffer_index = 0;

  static int word_count = 0;
  char word_output_dir[PATH_MAX];
  snprintf(word_output_dir, sizeof(word_output_dir), "%s/word_%d",
           output_directory, word_count++);
  mkdir(word_output_dir, 0777);

  char letter_full_path[PATH_MAX * 2];
  char letter_output_path[PATH_MAX * 2];

  for (int i = 0; i < letter_file_count; i++) {
    snprintf(letter_full_path, sizeof(letter_full_path), "%s/%s",
             letters_directory, letter_filenames[i]);
    snprintf(letter_output_path, sizeof(letter_output_path), "%s/%s",
             word_output_dir, letter_filenames[i]);

    FILE *src_file = fopen(letter_full_path, "rb");
    if (!src_file) {
      err(EXIT_FAILURE, "Couldn't open file %s for reading", letter_full_path);
    }

    FILE *dest_file = fopen(letter_output_path, "wb");
    if (!dest_file) {
      fclose(src_file);
      err(EXIT_FAILURE, "Couldn't open file %s for writing",
          letter_output_path);
    }

    char buffer[BUFSIZ];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFSIZ, src_file)) > 0) {
      fwrite(buffer, 1, bytes, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);

    char letter = recognize_letter(letter_full_path, nn);
    word_buffer[word_buffer_index++] = letter;
  }

  word_buffer[word_buffer_index] = '\0';
  fprintf(words_file, "%s\n", word_buffer);

  for (int i = 0; i < letter_file_count; i++) {
    free(letter_filenames[i]);
  }
  free(letter_filenames);

  DIR *dir = opendir(letters_directory);
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (strstr(entry->d_name, ".png")) {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", letters_directory, entry->d_name);
        unlink(path);
      }
    }
    closedir(dir);
  }
}

// Process a word image and extract letter images
void process_word_image(const char *full_path, NeuralNetwork *nn,
                        FILE *words_file) {
  iImage *image = load_image(full_path, -1);
  if (image == NULL) {
    fprintf(stderr, "Error while loading image %s\n", full_path);
    return;
  }

  bBoundingBox_size letterss = apply_groups_box(image);
  bBoundingBox *letters = letterss.boxes;
  int size = letterss.size;
  int letter_count = 0;

  for (int i = 0; i < size; i++) {
    iImage *letter_image =
        create_subimage(image, letters[i].min_x, letters[i].min_y,
                        letters[i].width, letters[i].height);
    if (letter_image == NULL) {
      fprintf(stderr, "error while creating subimage %d\n", i);
      continue;
    }

    char output_path[256];
    snprintf(output_path, sizeof(output_path),
             "resources/extracted/word_letters/%d.png", letter_count++);
    save_image(resize_image_with_white(letter_image, 32, 32), output_path);

    free_image(letter_image);
  }

  process_letters_in_word(nn, words_file);
  free_image(image);
}

// Process all word images in a directory
void process_all_word_images(NeuralNetwork *nn) {
  const char *words_directory = "resources/extracted/words";
  DIR *dir = opendir(words_directory);
  if (!dir) {
    err(EXIT_FAILURE, "Couldn't open directory %s", words_directory);
  }

  FILE *words_file = fopen("resources/extracted/txt_data/words.txt", "w");
  if (!words_file) {
    err(EXIT_FAILURE, "Couldn't open words.txt");
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strstr(entry->d_name, ".png")) {
      char full_path[PATH_MAX];
      snprintf(full_path, sizeof(full_path), "%s/%s", words_directory,
               entry->d_name);
      process_word_image(full_path, nn, words_file);
    }
  }

  closedir(dir);
  fclose(words_file);
}

// Main function to recognize words from images
int words_builder() {
  srand((unsigned int)time(NULL));

  NeuralNetwork *nn = create_neural_network();
  FILE *file_check = fopen("resources/cache/neural_network_weights.dat", "rb");
  if (file_check) {
    fclose(file_check);
    load_neural_network(nn, "resources/cache/neural_network_weights.dat");
  } else {
    printf("Couldn't load the weights file\n");
    return EXIT_FAILURE;
  }

  process_all_word_images(nn);
  free_neural_network(nn);
  return 0;
}
