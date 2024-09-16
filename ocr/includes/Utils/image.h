#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef IMAGE_H
#define IMAGE_H

typedef struct pPixel {
    Uint8 r, g, b;
    struct pPixel *matrix;
} pPixel;

typedef struct iImage {
    unsigned int height, width;
    char *path;
    pPixel **pixels;
} iImage;

int init_sdl();

SDL_Surface *load_surface(const char *image_path);

iImage *create_image(unsigned int width, unsigned int height, const char *image_path);

void extract_pixels(SDL_Surface *surface, iImage *img);

iImage *load_image(const char *path);

void free_image(iImage *image_path);

#endif