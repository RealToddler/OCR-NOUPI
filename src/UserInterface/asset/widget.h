#ifndef WIDGETS_H
#define WIDGETS_H

#include <gtk/gtk.h>

/**
 * @brief Initialize the control box
 * @param builder GTK Builder
 * @return GObject* Control box
 */
GObject *init_control_box(GtkBuilder *builder);

/**
 * @brief Show the control box
 */
void show_control_box();

/**
 * @brief Hide the control box
 */
void hide_control_box();

#endif