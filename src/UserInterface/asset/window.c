#include "window.h"
#include <gtk/gtk.h>

static GObject *window;

GObject *init_window(GtkBuilder *builder) {
  window = gtk_builder_get_object(builder, "window");
  if (window == NULL) {
    g_printerr("not UI file found\n");
    return NULL;
  }

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Get the default display
  GdkDisplay *display = gdk_display_get_default();
  if (display == NULL) {
    g_error("couldnt get default display");
    return NULL;
  }

  GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
  if (monitor == NULL) {
    g_error("error");
    return NULL;
  }

  // Get the monitor dimensions
  GdkRectangle geometry;
  gdk_monitor_get_geometry(monitor, &geometry);

  gint screen_width = geometry.width;
  gint screen_height = geometry.height;

  gint window_width = (5 * screen_width) / 7;
  gint window_height = (4 * screen_height) / 5;

  GtkWidget *window_widget = GTK_WIDGET(window);
  gtk_widget_set_size_request(window_widget, window_width, window_height);

  gtk_window_resize(GTK_WINDOW(window_widget), window_width, window_height);
  gint center_x = (screen_width - window_width) / 2;
  gint center_y = (screen_height - window_height) / 2;
  gtk_window_move(GTK_WINDOW(window_widget), center_x, center_y);

  return window;
}

GObject *get_window() { return window; }