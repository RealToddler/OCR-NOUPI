#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "SysCommand/syscmd.h"
#include "UserInterface/application.h"

/*
  Its name talks for itself
*/
int init_all() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("An error occured while initializing SDL : %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    printf("An error occured while initializing SDL_Image : %s\n",
           IMG_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*
  This function loads the app
*/
int main() {
  init_all();
  return open_app();
}