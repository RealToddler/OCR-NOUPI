#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Inclusions des bibliothèques pour la manipulation des images
#include "Image/resize.h"
#include "Image/image.h"
#include "Image/crop.h"

#include "Image/Preprocess/ColorTreatment/binary.h"
#include "Image/Preprocess/ColorTreatment/grayscale.h"

// Fonction pour parcourir les fichiers et sous-dossiers
void list_files_recursively(const char *base_path)
{
    struct dirent *entry;
    DIR *dir = opendir(base_path);

    // Vérifier si le dossier peut être ouvert
    if (dir == NULL)
    {
        printf("Erreur : Impossible d'ouvrir le dossier %s\n", base_path);
        return;
    }

    // Lire et parcourir les entrées du dossier
    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];

        // Ignorer "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Construire le chemin complet du fichier ou du dossier
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        // Vérifier si l'entrée est un fichier ou un dossier
        struct stat path_stat;
        stat(path, &path_stat);

        if (S_ISDIR(path_stat.st_mode))
        {
            // Si c'est un sous-dossier, appeler récursivement
            printf("Dossier : %s\n", path);
            list_files_recursively(path);
        }
        else
        {
            // Si c'est un fichier, traiter l'image
            printf("Fichier : %s\n", path);

            // Charger l'image
            iImage *img = load_image(path, -1);
            if (img == NULL)
            {
                printf("Erreur : Impossible de charger l'image %s\n", path);
                continue;
            }

             grayscale(img);
            binary(img);

            // Rogner l'image

            /*
            iImage *cropped = crop_image(img);
            if (cropped == NULL)
            {
                printf("Erreur : Échec du rognage de l'image %s\n", path);
                free_image(img);
                continue;
            }
            */

            // Redimensionner l'image
            iImage *resized = resize_image(img, 32, 32);
            if (resized == NULL)
            {
                printf("Erreur : Échec du redimensionnement de l'image %s\n", path);
                // free_image(cropped);
                free_image(img);
                continue;
            }

            save_image(resized, path);

            // Libérer les ressources des images
            free_image(resized);
            // free_image(cropped);
            free_image(img);
        }
    }

    // Fermer le dossier
    closedir(dir);
}

int main(int argc, char *argv[])
{
    // Vérifier si un chemin de dossier est passé en argument
    if (argc < 2)
    {
        printf("Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    // Appeler la fonction récursive pour lister les fichiers et les traiter
    list_files_recursively(argv[1]);

    return 0;
}
