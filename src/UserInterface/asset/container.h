#ifndef CONTAINER_H
#define CONTAINER_H

#include <gtk/gtk.h>

/**
 * @brief Initialize the image container
 * @param builder GTK Builder
 * @param window Main window
 * @return GObject* Image container
*/
GObject *container_init(GtkBuilder *builder, GtkWidget *window);

/**
 * @brief Load an image into the container
 * @param container Image container
 * @param source_image_path source image path
*/
void container_set_image(GtkWidget *container, const char *source_image_path);

/**
 * @brief Clear the image container
*/
void container_clear_image();

/**
 * @brief Get the image path
 * @param path Image path
*/
void get_image_path(char **path);

/**
 * @brief Get the image container
 * @return GtkWidget* Image container
*/
GtkWidget *get_image_container();

#endif