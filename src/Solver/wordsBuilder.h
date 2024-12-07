#ifndef WORDSBUILDER
#define WORDSBUILDER

#include "Image/Detection/canny.h"
#include "Image/Detection/cannyParameters.h"
#include "Image/Detection/extract.h"
#include "Image/image.h"
#include "NeuralNetwork/neuralNetwork.h"
#include "SysCommand/syscmd.h"

#define MAX_LINE_LENGTH 1024

int compare_filenames_bis(const void *a, const void *b);
char recognize_letter(const char *letter_full_path, NeuralNetwork *nn);
void process_letters_in_word(NeuralNetwork *nn, FILE *words_file);
void process_word_image(const char *full_path, NeuralNetwork *nn,
                        FILE *words_file);
void process_all_word_images(NeuralNetwork *nn);
int words_builder();

#endif // WORDSBUILDER