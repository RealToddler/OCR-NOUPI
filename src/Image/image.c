#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Image/image.h"

SDL_Surface *load_surface(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    if (surface == NULL) {
        printf("%s\n", image_path);
        printf("Erreur de chargement de l'image: %s\n", IMG_GetError());
    }
    return surface;
}

iImage *create_image(unsigned int width, unsigned int height,
                     const char *image_path) {
    iImage *img = (iImage *)malloc(sizeof(iImage));
    if (img == NULL) {
        printf("Erreur d'allocation mémoire pour l'image\n");
        return NULL;
    }

    img->width = width;
    img->height = height;

    img->path = (char *)malloc(strlen(image_path) + 1);
    if (img->path == NULL) {
        printf("Erreur d'allocation mémoire pour le chemin\n");
        free(img);
        return NULL;
    }
    strcpy(img->path, image_path);

    img->pixels = (pPixel **)malloc(height * sizeof(pPixel *));
    if (img->pixels == NULL) {
        printf("Erreur d'allocation mémoire pour les pixels\n");
        free(img->path);
        free(img);
        return NULL;
    }

    for (unsigned int i = 0; i < height; ++i) {
        img->pixels[i] = (pPixel *)malloc(width * sizeof(pPixel));
        if (img->pixels[i] == NULL) {
            printf("Erreur d'allocation mémoire pour la ligne de pixels %d\n",
                   i);
            for (unsigned int j = 0; j < i; ++j) {
                free(img->pixels[j]);
            }
            free(img->pixels);
            free(img->path);
            free(img);
            return NULL;
        }
    }

    return img;
}

void extract_pixels(SDL_Surface *surface, iImage *img) {
    SDL_LockSurface(surface);

    Uint8 *pixels = (Uint8 *)surface->pixels;
    int bpp = surface->format->BytesPerPixel;
    int pitch = surface->pitch;

    for (unsigned int y = 0; y < img->height; ++y) {
        for (unsigned int x = 0; x < img->width; ++x) {
            Uint8 *p = pixels + y * pitch + x * bpp;
            Uint32 pixel_value;

            switch (bpp) {
            case 1: pixel_value = *p; break;
            case 2: pixel_value = *(Uint16 *)p; break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    pixel_value = p[0] << 16 | p[1] << 8 | p[2];
                else
                    pixel_value = p[0] | p[1] << 8 | p[2] << 16;
                break;
            case 4: pixel_value = *(Uint32 *)p; break;
            default: pixel_value = 0; break;
            }

            Uint8 r, g, b;
            SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

            img->pixels[y][x].r = r;
            img->pixels[y][x].g = g;
            img->pixels[y][x].b = b;
        }
    }

    SDL_UnlockSurface(surface);
}

iImage *load_image(const char *image_path, int label) {
    SDL_Surface *surface = load_surface(image_path);
    if (surface == NULL) {
        return NULL;
    }

    iImage *img = create_image(surface->w, surface->h, image_path);
    img->label = label;
    if (img == NULL) {
        SDL_FreeSurface(surface);
        return NULL;
    }

    extract_pixels(surface, img);

    SDL_FreeSurface(surface);

    return img;
}

void save_image(iImage *img, const char *image_path) {
    if (img == NULL || img->pixels == NULL) {
        fprintf(stderr, "Erreur : image ou pixels invalide.\n");
        return;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, img->width, img->height, 24, SDL_PIXELFORMAT_RGB24);
    if (surface == NULL) {
        fprintf(stderr, "Erreur lors de la création de la surface SDL : %s\n",
                SDL_GetError());
        return;
    }

    SDL_LockSurface(surface);

    Uint8 *pixels = (Uint8 *)surface->pixels;
    int pitch = surface->pitch;

    for (unsigned int y = 0; y < img->height; y++) {
        for (unsigned int x = 0; x < img->width; x++) {
            pPixel current_pixel = img->pixels[y][x];
            Uint8 *p = pixels + y * pitch + x * 3;
            p[0] = current_pixel.r;
            p[1] = current_pixel.g;
            p[2] = current_pixel.b;
        }
    }

    SDL_UnlockSurface(surface);

    if (IMG_SavePNG(surface, image_path) != 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image PNG : %s\n",
                IMG_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_FreeSurface(surface);
    // printf("Image sauvegardée avec succès dans le fichier : %s\n", image_path);
}

void free_image(iImage *img) {
    if (img != NULL) {
        if (img->pixels != NULL) {
            for (unsigned int i = 0; i < img->height; ++i) {
                free(img->pixels[i]);
            }
            free(img->pixels);
        }
        if (img->path != NULL) {
            free(img->path);
        }
        free(img);
    }
}