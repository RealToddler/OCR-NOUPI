#ifndef WIDGETS_H
#define WIDGETS_H

#include <gtk/gtk.h>

void on_drag_data_received(GtkWidget *widget, GdkDragContext *context,
                            gint x, gint y, GtkSelectionData *data,
                            guint info, guint time, gpointer user_data);

void setup_drag_and_drop(GtkWidget *event_box, GtkWidget *label);

void on_drag_drop_zone_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
#endif