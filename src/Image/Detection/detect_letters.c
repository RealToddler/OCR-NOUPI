#include "boxes.h"
#include "detect_letters.h"
#include "../Image/image.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

cColor orange = {255, 165, 0};
cColor pink = {255, 192, 203};

void draw_group(iImage *image, int **visited, int y, int x, int *xi, int *yi, int *xf, int *yf);

int is_color2(iImage *image, int y, int x) {
    if (x < 0 || y < 0 || x >= image->width || y >= image->height)
        return 0;

    if ((image->pixels[y][x].r == 255 && image->pixels[y][x].g == 255 &&
        image->pixels[y][x].b == 255)||(image->pixels[y][x].r == 255 
        && image->pixels[y][x].g == 192 && image->pixels[y][x].b == 203))
        return 0;
    else
        return 1;
}


bBoundingBox_size apply_groups_box(iImage *image) {
    int size=1;
    bBoundingBox * res=malloc(size*sizeof(bBoundingBox));
    

    int **visited = malloc(image->height * sizeof(int *));
    if (visited == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < image->height; i++) {
        visited[i] =
            calloc(image->width,
                   sizeof(int)); // set visited to 0/false for each pixels
        if (visited[i] == NULL) {
            fprintf(stderr, "Memory allocation error[%d]\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (x < 0 || y < 0 || x >= image->width || y >= image->height) {
                printf("errx: detect_letters");
                
            }

            if (is_color2(image, y, x) == 1 && visited[y][x] == 0 ) {
                // int len = len_group(image, visited, y, x);
                int xi=x;
                int yi=y;
                int xf=x;
                int yf=y;

                draw_group(image, visited, y, x, &xi, &yi, &xf, &yf);
                
                if(xf-xi<yf-yi)
                {
                    int marge=((yf-yi)-(xf-xi))/2;
                    xf+=marge;
                    xi-=marge;
                    if (xi < 0) xi = 0;
                    if (xf >= image->width) xf = image->width - 1;
                
                }
                printf("done\n");
                //draw_rectangle(image, xi, yi, xf, yf, pink);
                bBoundingBox elt = {xi, xf, yi, yf, yf-yi, xf-xi, (yf-yi)*(xf-xi)};
                res[size-1]= elt;
                size++;
                res=realloc(res,size*sizeof(bBoundingBox));
                printf("%d, %d, %d, %d, \n", xi, yi, xf, yf);
                
                
            }
        }
    }

    for (int i = 0; i < image->height; i++) {
        free(visited[i]);
    }
    free(visited);
    bBoundingBox_size res2;
    res2.boxes=res;
    res2.size=size;
    return res2;
}



void draw_group(iImage *image, int **visited, int y, int x, int *xi, int *yi, int *xf, int *yf) {

    if (y < 0 || x < 0 || y >= image->height || x >= image->width ||
        visited[y][x] || is_color2(image, y, x) == 0 )
        return;

    visited[y][x] = 1; // pixel is visited
    
    

    if (x < *xi)
        *xi = x;
    if (x > *xf)
        *xf = x;
    if (y < *yi)
        *yi = y;
    if (y > *yf)
        *yf = y;
    

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if ((i != 0 || j != 0) && is_color2(image, y + i, x + j) == 1 ) {
                draw_group(image, visited, y + i, x + j, xi, yi, xf, yf);
            }
        }
    }
    
}