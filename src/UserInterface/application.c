#include "application.h"
#include "widget.h"
#include "container.h"
#include <gtk/gtk.h>

static void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    // Charger le fichier CSS
    gtk_css_provider_load_from_path(provider, "resources/ui/style.css", NULL);

    // Appliquer les styles à l'écran par défaut
    gtk_style_context_add_provider_for_screen(
        screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}

int open_app() {
    GtkBuilder *builder;
    GObject *window;
    GObject *central_box;
    GObject *drag_drop_zone;
    GObject *drag_drop_label;

    GError *error = NULL;

    // Initialisation GTK
    int argc = 0;
    gtk_init(&argc, NULL);

    load_css();

    // Chargement de l'interface UI
    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "resources/ui/builder.ui", &error) ==
        0) {
        g_printerr("Error while loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Récupération de la fenêtre
    window = gtk_builder_get_object(builder, "window");
    drag_drop_zone = gtk_builder_get_object(builder, "drag-drop-zone");
    drag_drop_label = gtk_builder_get_object(builder, "drag-drop-label");
    central_box = gtk_builder_get_object(builder, "central-box");

    if (!window || !drag_drop_zone || !drag_drop_label) {
        g_printerr("Error: 'window' object not found in UI file.\n");
        return 1;
    }

    container_init(GTK_WIDGET(drag_drop_zone), GTK_WIDGET(drag_drop_label));

    g_signal_connect(GTK_WIDGET(drag_drop_zone), "button-press-event",
                     G_CALLBACK(on_drag_drop_zone_clicked), window);

    g_object_set_data(G_OBJECT(drag_drop_zone), "drag-drop-label",
                      drag_drop_label);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Obtenir le display par défaut
    GdkDisplay *display = gdk_display_get_default();
    if (display == NULL) {
        g_error("Impossible d'obtenir le display par défaut.");
        return -1;
    }
    // Obtenir le monitor par défaut
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (monitor == NULL) {
        g_error("Impossible d'obtenir le monitor par défaut.");
        return -1;
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

    // Afficher la fenêtre
    gtk_widget_show_all(window_widget);

    gtk_main();

    return 0;
}
