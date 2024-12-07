#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>

/**
 * @brief Initialize the main window
 * @param builder GTK Builder
 * @return GObject* Main window
 */
GObject *init_window(GtkBuilder *builder);

/**
 * @brief Get the main window
 * @return GObject* Main window
 */
GObject *get_window();

#endif