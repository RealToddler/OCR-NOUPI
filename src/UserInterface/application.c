#include "application.h"
#include <gtk/gtk.h>

int open_app() {
  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  GError *error = NULL;

  int argc = 0;
  gtk_init(&argc, NULL);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "resources/ui/builder.ui", &error) 
    == 0) {
    g_printerr("Error while loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  window = gtk_builder_get_object(builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  gtk_main();
  return 0;
}
