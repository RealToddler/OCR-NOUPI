#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Fonction globale pour conserver l'image SDL actuelle
SDL_Surface *image_surface_global = NULL;

// Fonction pour convertir une image SDL en niveaux de gris
void convertir_en_gris(SDL_Surface *surface)
{
    int largeur = surface->w;
    int hauteur = surface->h;
    int canaux = surface->format->BytesPerPixel;
    unsigned char *pixels = (unsigned char *)surface->pixels;

    for (int y = 0; y < hauteur; ++y)
    {
        for (int x = 0; x < largeur; ++x)
        {
            int index = (y * largeur + x) * canaux;
            unsigned char r = pixels[index];
            unsigned char g = pixels[index + 1];
            unsigned char b = pixels[index + 2];

            unsigned char gris = (unsigned char)(0.3 * r + 0.59 * g + 0.11 * b);

            pixels[index] = gris;
            pixels[index + 1] = gris;
            pixels[index + 2] = gris;
        }
    }
}

// Fonction d'échange pour le tri (utile pour la médiane)
void echanger(unsigned char *a, unsigned char *b)
{
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}

// Fonction pour calculer la médiane d'un tableau
unsigned char calculer_mediane(unsigned char tableau[], int taille)
{
    // Tri du tableau pour trouver la médiane
    for (int i = 0; i < taille - 1; ++i)
    {
        for (int j = i + 1; j < taille; ++j)
        {
            if (tableau[i] > tableau[j])
            {
                echanger(&tableau[i], &tableau[j]);
            }
        }
    }
    return tableau[taille / 2]; // Retourne la valeur médiane
}

// Fonction pour appliquer un filtre médian
void filtrer_bruit_median(SDL_Surface *surface)
{
    int largeur = surface->w;
    int hauteur = surface->h;
    int canaux = surface->format->BytesPerPixel;
    unsigned char *pixels = (unsigned char *)surface->pixels;
    unsigned char *nouveaux_pixels = (unsigned char *)malloc(largeur * hauteur * canaux);

    int fenetre = 3; // Taille de la fenêtre de filtrage (3x3)

    for (int y = 1; y < hauteur - 1; ++y)
    {
        for (int x = 1; x < largeur - 1; ++x)
        {
            unsigned char voisins[9]; // Stocke les voisins (3x3)

            // Récupération des pixels voisins
            int k = 0;
            for (int j = -1; j <= 1; ++j)
            {
                for (int i = -1; i <= 1; ++i)
                {
                    int index = ((y + j) * largeur + (x + i)) * canaux;
                    voisins[k++] = pixels[index];
                }
            }

            // Calcul de la médiane
            unsigned char mediane = calculer_mediane(voisins, 9);
            int index_courant = (y * largeur + x) * canaux;

            // Applique la médiane à tous les canaux (gris)
            nouveaux_pixels[index_courant] = mediane;
            nouveaux_pixels[index_courant + 1] = mediane;
            nouveaux_pixels[index_courant + 2] = mediane;
        }
    }

    // Remplace les anciens pixels par les nouveaux
    memcpy(pixels, nouveaux_pixels, largeur * hauteur * canaux);
    free(nouveaux_pixels);
}

// Fonction pour une binarisation adaptative
void binarisation_adaptative(SDL_Surface *surface, int taille_bloc, double constante)
{
    int largeur = surface->w;
    int hauteur = surface->h;
    int canaux = surface->format->BytesPerPixel;
    unsigned char *pixels = (unsigned char *)surface->pixels;
    unsigned char *nouveaux_pixels = (unsigned char *)malloc(largeur * hauteur * canaux);

    for (int y = 0; y < hauteur; ++y)
    {
        for (int x = 0; x < largeur; ++x)
        {
            int somme = 0;
            int nombre_pixels = 0;

            // Calcul de la somme locale des pixels dans un bloc
            for (int j = -taille_bloc / 2; j <= taille_bloc / 2; ++j)
            {
                for (int i = -taille_bloc / 2; i <= taille_bloc / 2; ++i)
                {
                    int xx = x + i;
                    int yy = y + j;
                    if (xx >= 0 && xx < largeur && yy >= 0 && yy < hauteur)
                    {
                        int index = (yy * largeur + xx) * canaux;
                        somme += pixels[index];
                        nombre_pixels++;
                    }
                }
            }

            // Calcul de la moyenne locale
            double moyenne_locale = somme / (double)nombre_pixels;

            // Seuil adaptatif
            int index_courant = (y * largeur + x) * canaux;
            if (pixels[index_courant] < moyenne_locale - constante)
            {
                nouveaux_pixels[index_courant] = 0; // Noir
                nouveaux_pixels[index_courant + 1] = 0;
                nouveaux_pixels[index_courant + 2] = 0;
            }
            else
            {
                nouveaux_pixels[index_courant] = 255; // Blanc
                nouveaux_pixels[index_courant + 1] = 255;
                nouveaux_pixels[index_courant + 2] = 255;
            }
        }
    }

    // Remplace les anciens pixels par les nouveaux
    memcpy(pixels, nouveaux_pixels, largeur * hauteur * canaux);
    free(nouveaux_pixels);
}

// Gestionnaire d'événements pour ouvrir une image
void on_open_image(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Open Image", GTK_WINDOW(data),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Charger l'image avec SDL_image
        SDL_Surface *image_surface = IMG_Load(filename);
        if (!image_surface)
        {
            printf("Erreur de chargement de l'image : %s\n", IMG_GetError());
        }
        else
        {
            printf("Image chargée : %s\n", filename);

            // Appliquer les étapes de pré-traitement
            convertir_en_gris(image_surface);               // Conversion en niveaux de gris
            filtrer_bruit_median(image_surface);            // Filtrage médian
            // binarisation_adaptative(image_surface, 15, 10); // Binarisation adaptative avec bloc de 15x15 et constante de 10

            // Enregistrer l'image dans une surface globale pour la sauvegarde
            if (image_surface_global)
            {
                SDL_FreeSurface(image_surface_global);
            }
            image_surface_global = image_surface; // Garde l'image pour la sauvegarde

            // Afficher l'image avec SDL
            SDL_Window *window = SDL_CreateWindow("SDL2 Image",
                                                  SDL_WINDOWPOS_UNDEFINED,
                                                  SDL_WINDOWPOS_UNDEFINED,
                                                  image_surface->w,
                                                  image_surface->h,
                                                  SDL_WINDOW_SHOWN);
            SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image_surface);

            // Boucle de rendu SDL
            SDL_Event event;
            int quit = 0;
            while (!quit)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        quit = 1;
                    }
                }

                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }

            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
        }

        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Gestionnaire d'événements pour sauvegarder une image
void on_save_image(GtkWidget *widget, gpointer data)
{
    if (image_surface_global == NULL)
    {
        printf("Aucune image à sauvegarder\n");
        return;
    }

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save Image", GTK_WINDOW(data),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Sauvegarder l'image en format PNG
        if (IMG_SavePNG(image_surface_global, filename) != 0)
        {
            printf("Erreur de sauvegarde de l'image : %s\n", IMG_GetError());
        }
        else
        {
            printf("Image sauvegardée avec succès : %s\n", filename);
        }

        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Création de la fenêtre GTK avec boutons de contrôle
int main(int argc, char *argv[])
{
    // Initialisation de GTK
    gtk_init(&argc, &argv);

    // Création de la fenêtre
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "SDL2 + GTK Image Processing");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    // Gestion des signaux pour fermer la fenêtre
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Création de la boîte de boutons
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Bouton pour ouvrir une image
    GtkWidget *open_button = gtk_button_new_with_label("Ouvrir une image");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_image), window);
    gtk_box_pack_start(GTK_BOX(box), open_button, TRUE, TRUE, 0);

    // Bouton pour sauvegarder une image
    GtkWidget *save_button = gtk_button_new_with_label("Sauvegarder l'image");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_image), window);
    gtk_box_pack_start(GTK_BOX(box), save_button, TRUE, TRUE, 0);

    // Afficher tous les widgets
    gtk_widget_show_all(window);

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // Boucle principale GTK
    gtk_main();

    // Libérer la surface SDL lors de la fermeture
    if (image_surface_global)
    {
        SDL_FreeSurface(image_surface_global);
    }

    SDL_Quit();
    return 0;
}
