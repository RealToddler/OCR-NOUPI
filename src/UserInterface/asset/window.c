#include "window.h"
#include <gtk/gtk.h>

static GObject *window;

GObject *init_window(GtkBuilder *builder) {
    window = gtk_builder_get_object(builder, "window");
    if (window == NULL) {
        g_printerr("Error: window not found in UI file.\n");
        return NULL;
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Obtenir le display par défaut
    GdkDisplay *display = gdk_display_get_default();
    if (display == NULL) {
        g_error("Impossible d'obtenir le display par défaut.");
        return NULL;
    }

    // Obtenir le monitor par défaut
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (monitor == NULL) {
        g_error("Impossible d'obtenir le monitor par défaut.");
        return NULL;
    }

    // Obtenir les dimensions de l'écran
    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);

    gint screen_width = geometry.width;
    gint screen_height = geometry.height;

    // Calcul des dimensions de la fenêtre
    gint window_width = (5 * screen_width) / 7;
    gint window_height = (4 * screen_height) / 5;

    // Appliquer la taille minimale
    GtkWidget *window_widget = GTK_WIDGET(window);
    gtk_widget_set_size_request(window_widget, window_width, window_height);

    // Redimensionner et centrer la fenêtre
    gtk_window_resize(GTK_WINDOW(window_widget), window_width, window_height);
    gint center_x = (screen_width - window_width) / 2;
    gint center_y = (screen_height - window_height) / 2;
    gtk_window_move(GTK_WINDOW(window_widget), center_x, center_y);

    return window;
}