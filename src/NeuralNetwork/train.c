#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Image/image.h"
#include "formulas.h"
#include "neuralNetwork.h"
#include "train.h"

// Load dataset of labeled images from a directory
int load_dataset(const char *dataset_path, iImage **images) {
  int image_count = 0;
  char letter_dir[PATH_MAX];

  for (char letter = 'A'; letter <= 'Z'; letter++) {
    size_t required_size = snprintf(NULL, 0, "%s/%c", dataset_path, letter) + 1;
    if (required_size > sizeof(letter_dir)) {
      fprintf(stderr, "Path too long for letter_dir: %s/%c\n", dataset_path,
              letter);
      continue;
    }

    snprintf(letter_dir, sizeof(letter_dir), "%s/%c", dataset_path, letter);
    printf("%s\n", letter_dir);

    DIR *dir = opendir(letter_dir);
    if (dir == NULL) {
      printf("Could not open directory %s\n", letter_dir);
      continue;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

      char filepath[PATH_MAX + NAME_MAX + 2];
      size_t required_filepath_size =
          snprintf(NULL, 0, "%s/%s", letter_dir, entry->d_name) + 1;

      if (required_filepath_size > sizeof(filepath)) {
        fprintf(stderr, "Path too long for filepath: %s/%s\n", letter_dir,
                entry->d_name);
        continue;
      }

      snprintf(filepath, sizeof(filepath), "%s/%s", letter_dir, entry->d_name);

      int label = letter - 'A';
      printf("%s\n", filepath);

      iImage *image = load_image(filepath, label);
      if (image != NULL) {
        images[image_count++] = image;
        if (image_count >= MAX_IMAGES) {
          closedir(dir);
          return image_count;
        }
      }
    }
    closedir(dir);
  }

  return image_count;
}

// Shuffle the dataset randomly
void shuffle_images(iImage **images, int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    iImage *temp = images[i];
    images[i] = images[j];
    images[j] = temp;
  }
}

// Train the neural network using the dataset
int train() {
  srand((unsigned int)time(NULL));

  const double lr = 0.1f;

  NeuralNetwork *nn = create_neural_network();

  FILE *file_check = fopen(WEIGHTS_FILE, "rb");
  if (file_check) {
    fclose(file_check);
    load_neural_network(nn, WEIGHTS_FILE);
  } else {
    iImage *images[MAX_IMAGES];
    int total_images = load_dataset("dataset", images);

    if (total_images == 0) {
      printf("No images loaded.\n");
      return -1;
    }
    printf("Loaded %d images.\n", total_images);

    for (int epoch = 0; epoch < EPOCHS; epoch++) {
      shuffle_images(images, total_images);

      for (int x = 0; x < total_images; x++) {
        double input_layer[INPUTS_NUMBER];
        int idx = 0;
        for (int i = 0; i < images[x]->height; i++) {
          for (int j = 0; j < images[x]->width; j++) {
            Uint8 pixel_value = images[x]->pixels[i][j].r;
            input_layer[idx++] = pixel_value > 0 ? 1.0 : 0.0;
          }
        }

        double expected_output[OUTPUTS_NUMBER] = {0};
        expected_output[images[x]->label] = 1.0;

        forward_pass(nn, input_layer);
        backpropagation(nn, input_layer, expected_output, lr);
      }

      printf("epochs %d/%d done.\n", epoch + 1, EPOCHS);
    }

    save_neural_network(nn, WEIGHTS_FILE);

    for (int i = 0; i < total_images; i++) {
      for (int y = 0; y < images[i]->height; y++) {
        free(images[i]->pixels[y]);
      }
      free(images[i]->pixels);
      free(images[i]->path);
      free(images[i]);
    }
  }

  free_neural_network(nn);

  IMG_Quit();
  SDL_Quit();

  return 1;
}
