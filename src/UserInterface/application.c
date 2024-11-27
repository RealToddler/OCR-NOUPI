#include "application.h"
#include "asset/container.h"
#include "asset/menu.h"
#include "asset/widget.h"
#include "asset/window.h"
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

    window = init_window(builder);
    if (!window || !container_init(builder, GTK_WIDGET(window)) ||
        !menu_init(builder) || !init_control_box(builder)) {
        g_printerr("Error: one of defined object not found in UI file.\n");
        return 1;
    }

    gtk_widget_show_all(GTK_WIDGET(window));
    
    gtk_main();

    return 0;
}
