#ifndef MENU_H
#define MENU_H

#include <gtk/gtk.h>

/**
 * @brief Initialize the menu
 * @param builder GTK Builder
 * @return GObject* Menu revealer
 */
GObject *menu_init(GtkBuilder *builder);

#endif