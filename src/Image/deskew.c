#include "Image/deskew.h"
#include "Utils/image.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265

double compute_skew_angle(iImage *img)
{
    unsigned int width = img->width;
    unsigned int height = img->height;

    // Paramètres pour la recherche d'angle
    double angle_start = -100.0; // Angle minimum
    double angle_end = 100.0;    // Angle maximum
    double angle_step = 0.5;    // Pas d'angle

    double best_angle = 0.0;
    double max_variance = 0.0;

    // Coordonnées du centre de l'image
    double center_x = width / 2.0;
    double center_y = height / 2.0;

    // Parcourir les angles possibles
    for (double angle = angle_start; angle <= angle_end; angle += angle_step)
    {
        // Créer un histogramme de projection
        int *projection = (int *)calloc(height, sizeof(int));

        // Calculer le cosinus et le sinus de l'angle
        double rad = angle * PI / 180.0;
        double cos_a = cos(rad);
        double sin_a = sin(rad);

        // Parcourir les pixels de l'image
        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                pPixel *pixel = &img->pixels[y][x];

                // Si le pixel est noir (binaire)
                if (pixel->r == 0)
                {
                    // Déplacer les coordonnées pour que le centre soit l'origine
                    double x_shifted = x - center_x;
                    double y_shifted = y - center_y;

                    // **Corriger les équations de rotation**
                    int new_x = (int)(x_shifted * cos_a - y_shifted * sin_a);
                    int new_y = (int)(x_shifted * sin_a + y_shifted * cos_a);

                    // Replacer les coordonnées dans le système original
                    new_x += center_x;
                    new_y += center_y;

                    // Vérifier que les nouvelles coordonnées sont dans les limites
                    if (new_y >= 0 && new_y < (int)height)
                    {
                        projection[new_y]++;
                    }
                }
            }
        }

        // Calculer la variance de l'histogramme de projection
        double mean = 0.0;
        for (unsigned int i = 0; i < height; i++)
        {
            mean += projection[i];
        }
        mean /= height;

        double variance = 0.0;
        for (unsigned int i = 0; i < height; i++)
        {
            double diff = projection[i] - mean;
            variance += diff * diff;
        }

        // Garder l'angle avec la variance maximale
        if (variance > max_variance)
        {
            max_variance = variance;
            best_angle = angle;
        }

        free(projection);
    }

    return best_angle;
}

iImage *rotate_image(iImage *img, double angle)
{
    unsigned int width = img->width;
    unsigned int height = img->height;

    double rad = -angle * PI / 180.0; // Négatif pour corriger l'inclinaison
    double cos_a = cos(rad);
    double sin_a = sin(rad);

    // Calculer les dimensions de la nouvelle image
    unsigned int new_width = (unsigned int)(fabs(width * cos_a) + fabs(height * sin_a));
    unsigned int new_height = (unsigned int)(fabs(width * sin_a) + fabs(height * cos_a));

    // Créer la nouvelle image
    iImage *rotated_img = create_image(new_width, new_height, img->path);
    if (rotated_img == NULL)
    {
        printf("Erreur lors de la création de l'image pivotée.\n");
        return NULL;
    }

    // Initialiser les pixels de la nouvelle image en blanc
    for (unsigned int y = 0; y < new_height; y++)
    {
        for (unsigned int x = 0; x < new_width; x++)
        {
            rotated_img->pixels[y][x].r = 255;
            rotated_img->pixels[y][x].g = 255;
            rotated_img->pixels[y][x].b = 255;
        }
    }

    // Coordonnées du centre des images
    double cx = width / 2.0;
    double cy = height / 2.0;
    double ncx = new_width / 2.0;
    double ncy = new_height / 2.0;

    // Parcourir chaque pixel de la nouvelle image
    for (unsigned int y = 0; y < new_height; y++)
    {
        for (unsigned int x = 0; x < new_width; x++)
        {
            // Coordonnées dans la nouvelle image par rapport au centre
            double x_shifted = x - ncx;
            double y_shifted = y - ncy;

            // Calculer les coordonnées originales en utilisant la rotation inverse
            double orig_x = x_shifted * cos_a - y_shifted * sin_a + cx;
            double orig_y = x_shifted * sin_a + y_shifted * cos_a + cy;

            // Vérifier que les coordonnées originales sont dans les limites
            if (orig_x >= 0 && orig_x < width && orig_y >= 0 && orig_y < height)
            {
                // Interpolation par voisin le plus proche
                unsigned int ix = (unsigned int)(orig_x);
                unsigned int iy = (unsigned int)(orig_y);

                rotated_img->pixels[y][x] = img->pixels[iy][ix];
            }
        }
    }

    return rotated_img;
}

iImage *deskew_image(iImage *img)
{
    // Vérifier que l'image est binarisée
    // Si ce n'est pas le cas, appliquer la binarisation
    // binary(img);

    // Calculer l'angle d'inclinaison
    double angle = compute_skew_angle(img);
    printf("Angle d'inclinaison détecté : %f degrés\n", angle);

    // Faire pivoter l'image pour la redresser
    iImage *deskewed_img = rotate_image(img, angle);

    return deskewed_img;
}
