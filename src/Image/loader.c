#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "Utils/image.h"
#include "Preprocess/binary.h"
#include "Preprocess/grayscale.h"

int init_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Erreur d'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    return 0;
}

SDL_Surface *load_surface(const char *image_path)
{
    SDL_Surface *surface = IMG_Load(image_path);
    if (surface == NULL)
    {
        printf("Erreur de chargement de l'image: %s\n", IMG_GetError());
    }
    return surface;
}

iImage *create_image(unsigned int width, unsigned int height, const char *image_path)
{
    iImage *img = (iImage *)malloc(sizeof(iImage));
    if (img == NULL)
    {
        printf("Erreur d'allocation mémoire pour l'image\n");
        return NULL;
    }

    img->width = width;
    img->height = height;

    img->path = (char *)malloc(strlen(image_path) + 1);
    if (img->path == NULL)
    {
        printf("Erreur d'allocation mémoire pour le chemin\n");
        free(img);
        return NULL;
    }
    strcpy(img->path, image_path);

    img->pixels = (pPixel **)malloc(height * sizeof(pPixel *));
    if (img->pixels == NULL)
    {
        printf("Erreur d'allocation mémoire pour les pixels\n");
        free(img->path);
        free(img);
        return NULL;
    }

    for (unsigned int i = 0; i < height; ++i)
    {
        img->pixels[i] = (pPixel *)malloc(width * sizeof(pPixel));
        if (img->pixels[i] == NULL)
        {
            printf("Erreur d'allocation mémoire pour la ligne de pixels %d\n", i);
            for (unsigned int j = 0; j < i; ++j)
            {
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

void extract_pixels(SDL_Surface *surface, iImage *img)
{
    Uint32 *sdl_pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4;

    for (unsigned int y = 0; y < img->height; ++y)
    {
        for (unsigned int x = 0; x < img->width; ++x)
        {
            unsigned int pixel_value = sdl_pixels[y * pitch + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel_value, surface->format, &r, &g, &b);

            img->pixels[y][x].r = r;
            img->pixels[y][x].g = g;
            img->pixels[y][x].b = b;
        }
    }
}

iImage *load_image(const char *image_path)
{
    if (init_sdl() != 0)
    {
        return NULL;
    }

    SDL_Surface *surface = load_surface(image_path);
    if (surface == NULL)
    {
        IMG_Quit();
        SDL_Quit();
        return NULL;
    }

    iImage *img = create_image(surface->w, surface->h, image_path);
    if (img == NULL)
    {
        SDL_FreeSurface(surface);
        IMG_Quit();
        SDL_Quit();
        return NULL;
    }

    extract_pixels(surface, img);

    SDL_FreeSurface(surface);

    IMG_Quit();
    SDL_Quit();

    return img;
}

void save_image(iImage *img, const char *image_path)
{
    if (img == NULL || img->pixels == NULL)
    {
        fprintf(stderr, "Erreur : image ou pixels invalide.\n");
        return;
    }

    // Création d'une surface SDL pour contenir l'image en format RGB
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, img->width, img->height, 24, SDL_PIXELFORMAT_RGB24);
    if (surface == NULL)
    {
        fprintf(stderr, "Erreur lors de la création de la surface SDL : %s\n", SDL_GetError());
        return;
    }

    // Remplir la surface SDL avec les pixels de l'image
    for (unsigned int y = 0; y < img->height; y++)
    {
        for (unsigned int x = 0; x < img->width; x++)
        {
            // Accéder au pixel dans la surface SDL
            Uint32 *target_pixel = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch + x * 3);
            // Obtenir le pixel depuis iImage
            pPixel current_pixel = img->pixels[y][x];

            // Encoder le pixel en format RGB pour SDL
            Uint32 rgb_pixel = SDL_MapRGB(surface->format, current_pixel.r, current_pixel.g, current_pixel.b);

            // Stocker le pixel dans la surface SDL
            *target_pixel = rgb_pixel;
        }
    }

    // Sauvegarde de la surface en PNG
    if (IMG_SavePNG(surface, image_path) != 0)
    {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image PNG : %s\n", IMG_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    // Libérer la surface
    SDL_FreeSurface(surface);
    printf("Image sauvegardée avec succès dans le fichier : %s\n", image_path);
}

void free_image(iImage *img)
{
    if (img != NULL)
    {
        if (img->pixels != NULL)
        {
            for (unsigned int i = 0; i < img->height; ++i)
            {
                free(img->pixels[i]);
            }
            free(img->pixels);
        }
        if (img->path != NULL)
        {
            free(img->path);
        }
        free(img);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <path_to_image> <output_image>\n", argv[0]);
        return 1;
    }

    // Charger l'image
    iImage *img = load_image(argv[1]);
    if (img != NULL)
    {
        printf("Image chargée avec succès : %dx%d pixels\n", img->width, img->height);
        grayscale(img);
        //binary(img);
        save_image(img, argv[2]);
        free_image(img);
    }

    return 0;
}
