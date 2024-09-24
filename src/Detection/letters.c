#include "Utils/image.h"

#define PI 3.14

typedef struct
{
    int min_x;
    int max_x;
    int min_y;
    int max_y;
} BoundingBox;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

// Calcul des gradients avec le filtre de Sobel
void calculate_gradients(iImage *img, float **gradient_magnitude, float **gradient_direction)
{
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};
    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            int gx_r = 0, gy_r = 0;
            int gx_g = 0, gy_g = 0;
            int gx_b = 0, gy_b = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    gx_r += img->pixels[y + ky][x + kx].r * Gx[ky + 1][kx + 1];
                    gy_r += img->pixels[y + ky][x + kx].r * Gy[ky + 1][kx + 1];

                    gx_g += img->pixels[y + ky][x + kx].g * Gx[ky + 1][kx + 1];
                    gy_g += img->pixels[y + ky][x + kx].g * Gy[ky + 1][kx + 1];

                    gx_b += img->pixels[y + ky][x + kx].b * Gx[ky + 1][kx + 1];
                    gy_b += img->pixels[y + ky][x + kx].b * Gy[ky + 1][kx + 1];
                }
            }

            // Calcul de la magnitude et de la direction du gradient (on simplifie en moyenne)
            float grad_r = sqrt(gx_r * gx_r + gy_r * gy_r);
            float grad_g = sqrt(gx_g * gx_g + gy_g * gy_g);
            float grad_b = sqrt(gx_b * gx_b + gy_b * gy_b);

            gradient_magnitude[y][x] = (grad_r + grad_g + grad_b) / 3.0;

            gradient_direction[y][x] = atan2(gy_r + gy_g + gy_b, gx_r + gx_g + gx_b) * 180 / PI;
        }
    }
}

// Suppression des non-maxima
void non_max_suppression(iImage *img, float **gradient_magnitude, float **gradient_direction, float **edges)
{
    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            float direction = gradient_direction[y][x];
            float magnitude = gradient_magnitude[y][x];
            float mag1, mag2;

            // Approximations des directions (0°, 45°, 90°, 135°)
            if ((direction >= -22.5 && direction <= 22.5) || (direction >= 157.5 || direction <= -157.5))
            {
                mag1 = gradient_magnitude[y][x - 1];
                mag2 = gradient_magnitude[y][x + 1];
            }
            else if ((direction > 22.5 && direction <= 67.5) || (direction < -112.5 && direction >= -157.5))
            {
                mag1 = gradient_magnitude[y - 1][x + 1];
                mag2 = gradient_magnitude[y + 1][x - 1];
            }
            else if ((direction > 67.5 && direction <= 112.5) || (direction < -67.5 && direction >= -112.5))
            {
                mag1 = gradient_magnitude[y - 1][x];
                mag2 = gradient_magnitude[y + 1][x];
            }
            else
            {
                mag1 = gradient_magnitude[y - 1][x - 1];
                mag2 = gradient_magnitude[y + 1][x + 1];
            }

            // Suppression des non-maxima
            if (magnitude >= mag1 && magnitude >= mag2)
            {
                edges[y][x] = magnitude;
            }
            else
            {
                edges[y][x] = 0;
            }
        }
    }
}

// Hystérésis
void hysteresis_recursive(unsigned char **edge_map, unsigned int y, unsigned int x, unsigned int height, unsigned int width)
{
    // Parcours des 8 voisins
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dy == 0 && dx == 0)
                continue;
            int ny = y + dy;
            int nx = x + dx;
            if (ny >= 0 && ny < height && nx >= 0 && nx < width)
            {
                if (edge_map[ny][nx] == 1)
                {
                    edge_map[ny][nx] = 2; // Marquer comme bord fort
                    hysteresis_recursive(edge_map, ny, nx, height, width);
                }
            }
        }
    }
}

// Seuillage par hystérésis
void hysteresis_thresholding(iImage *img, float **edges, float low_thresh, float high_thresh, unsigned char **edge_map)
{
    // Initialisation
    for (unsigned int y = 0; y < img->height; y++)
    {
        for (unsigned int x = 0; x < img->width; x++)
        {
            edge_map[y][x] = 0;
        }
    }

    // Marquage des pixels
    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            if (edges[y][x] >= high_thresh)
            {
                edge_map[y][x] = 2; // Bord fort
            }
            else if (edges[y][x] >= low_thresh)
            {
                edge_map[y][x] = 1; // Bord faible
            }
            else
            {
                edge_map[y][x] = 0; // Non-bord
            }
        }
    }

    // Suivi des bords
    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            if (edge_map[y][x] == 2)
            {
                hysteresis_recursive(edge_map, y, x, img->height, img->width);
            }
        }
    }

    // Suppression des bords faibles non connectés
    for (unsigned int y = 0; y < img->height; y++)
    {
        for (unsigned int x = 0; x < img->width; x++)
        {
            if (edge_map[y][x] == 1)
            {
                edge_map[y][x] = 0;
            }
            else if (edge_map[y][x] == 2)
            {
                edge_map[y][x] = 1; // Bords finaux
            }
        }
    }
}

// Dilation
void dilate(unsigned char **input, unsigned char **output, unsigned int height, unsigned int width)
{
    // Initialisation de la sortie
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            output[y][x] = 0;
        }
    }

    // Dilation avec un élément structurant 3x3
    for (unsigned int y = 1; y < height - 1; y++)
    {
        for (unsigned int x = 1; x < width - 1; x++)
        {
            if (input[y][x] == 1)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        output[y + dy][x + dx] = 1;
                    }
                }
            }
        }
    }
}

// Fusion des bounding boxes qui se chevauchent ou sont proches
void merge_bounding_boxes(BoundingBox *boxes, int *num_boxes)
{
    int merged = 1;
    while (merged)
    {
        merged = 0;
        for (int i = 0; i < *num_boxes; i++)
        {
            for (int j = i + 1; j < *num_boxes; j++)
            {
                // Vérifier si les bounding boxes se chevauchent ou sont proches
                int overlap_x = (boxes[i].min_x <= boxes[j].max_x + 5) && (boxes[j].min_x <= boxes[i].max_x + 5);
                int overlap_y = (boxes[i].min_y <= boxes[j].max_y + 5) && (boxes[j].min_y <= boxes[i].max_y + 5);

                if (overlap_x && overlap_y)
                {
                    // Fusionner les bounding boxes
                    boxes[i].min_x = (boxes[i].min_x < boxes[j].min_x) ? boxes[i].min_x : boxes[j].min_x;
                    boxes[i].max_x = (boxes[i].max_x > boxes[j].max_x) ? boxes[i].max_x : boxes[j].max_x;
                    boxes[i].min_y = (boxes[i].min_y < boxes[j].min_y) ? boxes[i].min_y : boxes[j].min_y;
                    boxes[i].max_y = (boxes[i].max_y > boxes[j].max_y) ? boxes[i].max_y : boxes[j].max_y;

                    // Supprimer la bounding box fusionnée
                    for (int k = j; k < *num_boxes - 1; k++)
                    {
                        boxes[k] = boxes[k + 1];
                    }
                    (*num_boxes)--;
                    merged = 1;
                    break;
                }
            }
            if (merged)
                break;
        }
    }
}

// Flood Fill pour le marquage des composantes connexes
void flood_fill(unsigned char **edge_map, int **label_map, unsigned int x, unsigned int y, unsigned int height, unsigned int width, int label, BoundingBox *box)
{
    typedef struct
    {
        int x;
        int y;
    } Point;

    Point *stack = (Point *)malloc(height * width * sizeof(Point));
    int stack_size = 0;
    stack[stack_size].x = x;
    stack[stack_size].y = y;
    stack_size++;

    label_map[y][x] = label;

    while (stack_size > 0)
    {
        stack_size--;
        int cx = stack[stack_size].x;
        int cy = stack[stack_size].y;

        // Mise à jour de la bounding box
        if (cx < box->min_x)
            box->min_x = cx;
        if (cx > box->max_x)
            box->max_x = cx;
        if (cy < box->min_y)
            box->min_y = cy;
        if (cy > box->max_y)
            box->max_y = cy;

        // Vérification des voisins
        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                int nx = cx + dx;
                int ny = cy + dy;
                if (dx == 0 && dy == 0)
                    continue;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                {
                    if (edge_map[ny][nx] == 1 && label_map[ny][nx] == 0)
                    {
                        label_map[ny][nx] = label;
                        stack[stack_size].x = nx;
                        stack[stack_size].y = ny;
                        stack_size++;
                    }
                }
            }
        }
    }

    free(stack);
}

// Trouver les bounding boxes des composantes connexes
void find_bounding_boxes(unsigned char **edge_map, unsigned int height, unsigned int width, BoundingBox **boxes, int *num_boxes)
{
    int **label_map = (int **)malloc(height * sizeof(int *));
    for (unsigned int i = 0; i < height; i++)
    {
        label_map[i] = (int *)malloc(width * sizeof(int));
        for (unsigned int j = 0; j < width; j++)
        {
            label_map[i][j] = 0;
        }
    }

    int label = 1;
    *num_boxes = 0;
    BoundingBox *temp_boxes = (BoundingBox *)malloc(sizeof(BoundingBox) * height * width / 10);

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            if (edge_map[y][x] == 1 && label_map[y][x] == 0)
            {
                BoundingBox box;
                box.min_x = x;
                box.max_x = x;
                box.min_y = y;
                box.max_y = y;

                flood_fill(edge_map, label_map, x, y, height, width, label, &box);

                temp_boxes[*num_boxes] = box;
                (*num_boxes)++;
                label++;
            }
        }
    }

    *boxes = (BoundingBox *)malloc(sizeof(BoundingBox) * (*num_boxes));
    for (int i = 0; i < *num_boxes; i++)
    {
        (*boxes)[i] = temp_boxes[i];
    }

    free(temp_boxes);

    for (unsigned int i = 0; i < height; i++)
    {
        free(label_map[i]);
    }
    free(label_map);
}

// Dessiner un rectangle sur l'image
void draw_rectangle(iImage *img, int min_x, int min_y, int max_x, int max_y, Color color)
{
    // Dessiner les lignes horizontales
    for (int x = min_x; x <= max_x; x++)
    {
        if (min_y >= 0 && min_y < img->height)
        {
            img->pixels[min_y][x].r = color.r;
            img->pixels[min_y][x].g = color.g;
            img->pixels[min_y][x].b = color.b;
        }
        if (max_y >= 0 && max_y < img->height)
        {
            img->pixels[max_y][x].r = color.r;
            img->pixels[max_y][x].g = color.g;
            img->pixels[max_y][x].b = color.b;
        }
    }
    // Dessiner les lignes verticales
    for (int y = min_y; y <= max_y; y++)
    {
        if (min_x >= 0 && min_x < img->width)
        {
            img->pixels[y][min_x].r = color.r;
            img->pixels[y][min_x].g = color.g;
            img->pixels[y][min_x].b = color.b;
        }
        if (max_x >= 0 && max_x < img->width)
        {
            img->pixels[y][max_x].r = color.r;
            img->pixels[y][max_x].g = color.g;
            img->pixels[y][max_x].b = color.b;
        }
    }
}

// Fonction principale pour appliquer Canny
void apply_canny(iImage *img)
{
    // Étape 1 : Conversion en niveaux de gris (si nécessaire)

    // Étape 2 : Calcul des gradients
    float **gradient_magnitude = (float **)malloc(img->height * sizeof(float *));
    float **gradient_direction = (float **)malloc(img->height * sizeof(float *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        gradient_magnitude[i] = (float *)malloc(img->width * sizeof(float));
        gradient_direction[i] = (float *)malloc(img->width * sizeof(float));
    }
    calculate_gradients(img, gradient_magnitude, gradient_direction);

    // Étape 3 : Suppression des non-maxima
    float **edges = (float **)malloc(img->height * sizeof(float *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        edges[i] = (float *)malloc(img->width * sizeof(float));
    }
    non_max_suppression(img, gradient_magnitude, gradient_direction, edges);

    // Étape 4 : Seuillage par hystérésis
    unsigned char **edge_map = (unsigned char **)malloc(img->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        edge_map[i] = (unsigned char *)malloc(img->width * sizeof(unsigned char));
    }
    float low_thresh = 20.0;
    float high_thresh = 50.0;
    hysteresis_thresholding(img, edges, low_thresh, high_thresh, edge_map);

    // Étape 5 : Dilatation pour combler les lacunes
    unsigned char **dilated_edge_map = (unsigned char **)malloc(img->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        dilated_edge_map[i] = (unsigned char *)malloc(img->width * sizeof(unsigned char));
    }
    dilate(edge_map, dilated_edge_map, img->height, img->width);

    // Étape 6 : Trouver les bounding boxes
    BoundingBox *boxes;
    int num_boxes;
    find_bounding_boxes(dilated_edge_map, img->height, img->width, &boxes, &num_boxes);

    // Étape 7 : Fusionner les bounding boxes
    merge_bounding_boxes(boxes, &num_boxes);

    // Étape 8 : Dessiner les rectangles sur l'image
    Color red = {255, 0, 0}; // Couleur rouge pour les rectangles
    for (int i = 0; i < num_boxes; i++)
    {
        // Filtrer les bounding boxes trop petites pour éliminer le bruit
        int width = boxes[i].max_x - boxes[i].min_x;
        int height = boxes[i].max_y - boxes[i].min_y;
        if (width > 5 && height > 5)
        {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x, boxes[i].max_y, red);
        }
    }

    // Nettoyage mémoire
    for (unsigned int i = 0; i < img->height; i++)
    {
        free(gradient_magnitude[i]);
        free(gradient_direction[i]);
        free(edges[i]);
        free(edge_map[i]);
        free(dilated_edge_map[i]);
    }
    free(gradient_magnitude);
    free(gradient_direction);
    free(edges);
    free(edge_map);
    free(dilated_edge_map);
    free(boxes);
}
