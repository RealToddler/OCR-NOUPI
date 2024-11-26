#include "application.h"
#include <gtk/gtk.h>

static void load_css() {
  GtkCssProvider *provider = gtk_css_provider_new();
  GdkDisplay *display = gdk_display_get_default();
  GdkScreen *screen = gdk_display_get_default_screen(display);

  // Charger le fichier CSS
  gtk_css_provider_load_from_path(provider, "resources/ui/style.css", NULL);

  // Appliquer les styles à l'écran par défaut
  gtk_style_context_add_provider_for_screen(
      screen,
      GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_USER
      );

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
  if (gtk_builder_add_from_file(builder, "resources/ui/builder.ui", &error) == 0) {
    g_printerr("Error while loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  // Récupération de la fenêtre
  window = gtk_builder_get_object(builder, "window");
  if (!window) {
    g_printerr("Error: 'window' object not found in UI file.\n");
    return 1;
  }

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Dimensions de l'écran
  GdkScreen *screen = gdk_screen_get_default();
  gint screen_width = gdk_screen_get_width(screen);
  gint screen_height = gdk_screen_get_height(screen);

  // Calcul des dimensions de la fenêtre
  gint window_width = (5 * screen_width) / 7;
  gint window_height = (4 * screen_height) / 5;

  // Appliquer la taille minimale
  GtkWidget *window_widget = GTK_WIDGET(window);
  gtk_widget_set_size_request(window_widget, window_width, window_height);

  // Redimensionner et forcer le centrage
  gtk_window_resize(GTK_WINDOW(window_widget), window_width, window_height);

  // Calculer la position centrale
  gint center_x = (screen_width - window_width) / 2;
  gint center_y = (screen_height - window_height) / 2;

  // Déplacer la fenêtre au centre
  gtk_window_move(GTK_WINDOW(window_widget), center_x, center_y);

  // Afficher la fenêtre
  gtk_widget_show_all(window_widget);

  gtk_main();

  return 0;
}

