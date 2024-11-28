#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Image/image.h"

int init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n",
                SDL_GetError());
        return EXIT_FAILURE;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL_Image: %s\n",
                IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
    Loads a surface from a path using SDL2
*/
SDL_Surface *load_surface(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    if (surface == NULL) {
        printf("%s\n", image_path);
        printf("Erreur de chargement de l'image: %s\n", IMG_GetError());
    }
    return surface;
}

/*
    Creates an image from a path
*/
iImage *create_image(unsigned int width, unsigned int height,
                     const char *image_path) {
    iImage *img = (iImage *)malloc(sizeof(iImage));
    if (img == NULL) {
        printf("An error occured while allocating memory.\n");
        return NULL;
    }

    img->width = width;
    img->height = height;

    img->path = (char *)malloc(strlen(image_path) + 1);
    if (img->path == NULL) {
        printf(
            "An error occured while allocating memory for the path's data.\n");
        free(img);
        return NULL;
    }
    strcpy(img->path, image_path);

    img->pixels = (pPixel **)malloc(height * sizeof(pPixel *));
    if (img->pixels == NULL) {
        printf("An error occured while allocating memory for the pixels's "
               "data. \n");
        free(img->path);
        free(img);
        return NULL;
    }

    for (unsigned int i = 0; i < height; ++i) {
        img->pixels[i] = (pPixel *)malloc(width * sizeof(pPixel));
        if (img->pixels[i] == NULL) {
            printf("An error occured while allocating memory for the pixels's "
                   "line, %d\n",
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

/*
    Extracts pixels from SDL surface and copy them in a iImage struct
*/
void extract_pixels(SDL_Surface *surface, iImage *img) {
    SDL_LockSurface(surface);

    Uint8 *pixels = (Uint8 *)surface->pixels;
    int bpp = surface->format->BytesPerPixel;
    int pitch = surface->pitch;

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
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

/*
    Loads an image from its path and assign a label
*/
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

/*
    Saves an image at a given path
*/
void save_image(iImage *img, const char *image_path) {
    if (img == NULL || img->pixels == NULL) {
        fprintf(stderr, "Error with pixels or image.\n");
        return;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, img->width, img->height, 24, SDL_PIXELFORMAT_RGB24);
    if (surface == NULL) {
        fprintf(stderr, "Error with surface : %s\n", SDL_GetError());
        return;
    }

    SDL_LockSurface(surface);

    Uint8 *pixels = (Uint8 *)surface->pixels;
    int pitch = surface->pitch;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            pPixel current_pixel = img->pixels[y][x];
            Uint8 *p = pixels + y * pitch + x * 3;
            p[0] = current_pixel.r;
            p[1] = current_pixel.g;
            p[2] = current_pixel.b;
        }
    }

    SDL_UnlockSurface(surface);

    if (IMG_SavePNG(surface, image_path) != 0) {
        fprintf(stderr, "Error while saving the image : %s\n", IMG_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_FreeSurface(surface);
}

/*
    Frees an image
*/
void free_image(iImage *img) {
    if (img != NULL) {
        if (img->pixels != NULL) {
            for (int i = 0; i < img->height; ++i) {
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

/*
    Crops a region from the original image and creates a new image
*/
iImage *crop_image_cord(iImage *original_img, int x, int y, int width, int height) {
    // Check if the coordinates are within the bounds of the original image
    if (x < 0 || y < 0 || x + width > original_img->width ||
        y + height > original_img->height) {
        fprintf(stderr, "Crop coordinates are out of bounds.\n");
        return NULL;
    }

    // Create a new image with the specified width and height
    iImage *cropped_img = create_image(width, height, original_img->path);
    if (cropped_img == NULL) {
        fprintf(stderr, "Failed to create a new image.\n");
        return NULL;
    }

    // Copy the pixels from the original image to the new image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            cropped_img->pixels[i][j] = original_img->pixels[y + i][x + j];
        }
    }

    // Assign the label from the original image (optional)
    cropped_img->label = original_img->label;

    return cropped_img;
}

iImage *create_subimage(const iImage *original, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    // Vérification des paramètres d'entrée
    if (original == NULL) {
        fprintf(stderr, "Erreur : L'image originale est NULL.\n");
        return NULL;
    }

    if ((const int)(x + width) > original->width || (const int)(y + height) > original->height) {
        printf("%d + %d > %d || %d + %d > %d", x , width , original->width , y , height , original->height);
        fprintf(stderr, "Erreur : Les coordonnées de découpe dépassent les dimensions de l'image originale.\n");
        return NULL;
    }

    // Création de la nouvelle image
    iImage *subimg = create_image(width, height, original->path);
    if (subimg == NULL) {
        fprintf(stderr, "Erreur : Échec de la création de la nouvelle image.\n");
        return NULL;
    }

    // Copier les pixels de la région spécifiée
    for (unsigned int row = 0; row < height; ++row) {
        for (unsigned int col = 0; col < width; ++col) {
            subimg->pixels[row][col] = original->pixels[y + row][x + col];
        }
    }

    // Copier le label si nécessaire
    subimg->label = original->label;

    return subimg;
}

    


void to_red(iImage *image, int **visited, int y, int x);

int is_color3(iImage *image, int y, int x) {
    if (x < 0 || y < 0 || x >= image->width || y >= image->height)
        return 0;
    if (!(image->pixels[y][x].r == 255 && image->pixels[y][x].g == 255 &&
          image->pixels[y][x].b == 255))
        return 1;
    else
        return 0;
}


void to_red_func(iImage *image, int xi, int yi, int xf, int yf) {
    int **visited = malloc(image->height * sizeof(int *));
    if (visited == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < image->height; i++) {
        visited[i] = calloc(image->width, sizeof(int));
        if (visited[i] == NULL) {
            fprintf(stderr, "Memory allocation error[%d]\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int y = yi; y < yf; y++) {
        for (int x = xi; x < xf; x++) {
            if (x < 0 || y < 0 || x >= image->width || y >= image->height) {
                printf("Error: out of bounds in to_red_func\n");
                exit(EXIT_FAILURE);
            }

            if (is_color3(image, y, x) && visited[y][x] == 0) {
                to_red(image, visited, y, x);
                printf("done\n");
            }
        }
    }

    for (int i = 0; i < image->height; i++) {
        free(visited[i]);
    }
    free(visited);
}




void to_red(iImage *image, int **visited, int y, int x) {
    if (y < 0 || x < 0 || y >= image->height || x >= image->width ||
        visited[y][x] || is_color3(image, y, x) == 1)
        return;

    image->pixels[y][x].r=255;
    image->pixels[y][x].g=0;
    image->pixels[y][x].b=0;

    visited[y][x] = 1;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {
                int t = y + i;
                int u = x + j;

                if (t >= 0 && t < image->height && u >= 0 && u < image->width) {
                    if (visited[t][u] == 0 && is_color3(image, t, u) != 1) {
                        to_red(image, visited, t, u);
                    }
                }
            }
        }
    }
}


