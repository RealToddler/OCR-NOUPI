#include "menu.h"
#include <gtk/gtk.h>

// Callback pour ouvrir/fermer le menu
static void on_menu_toggle_button_clicked(GtkWidget *button,
                                          gpointer user_data) {
    GtkRevealer *menu_revealer = GTK_REVEALER(user_data);
    gboolean is_revealed = gtk_revealer_get_reveal_child(menu_revealer);
    gtk_revealer_set_reveal_child(menu_revealer, !is_revealed);
}

int menu_init(GtkBuilder *builder) {
    GObject *menu_toggle_button =
        gtk_builder_get_object(builder, "menu-toggle-button");
    GObject *menu_revealer = gtk_builder_get_object(builder, "menu-revealer");

    g_signal_connect(GTK_WIDGET(menu_toggle_button), "clicked",
                     G_CALLBACK(on_menu_toggle_button_clicked),
                     GTK_WIDGET(menu_revealer));
}