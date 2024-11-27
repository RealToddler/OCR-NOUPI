#ifndef CONTAINER_H
#define CONTAINER_H

#include <gtk/gtk.h>

// Initialiser le conteneur pour le drag-and-drop et l'affichage des images
void container_init(GtkWidget *container, GtkWidget *label);

// Charger une image dans le conteneur
void container_set_image(GtkWidget *container, const char *image_path);

#endif