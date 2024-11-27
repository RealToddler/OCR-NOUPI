#include "widget.h"
#include "container.h"
#include <gtk/gtk.h>

static GtkWidget *control_box;

static void on_clear_button_clicked(GtkWidget *widget, gpointer user_data) {
    container_clear_image();
}

static void on_solve_button_clicked(GtkWidget *widget, gpointer user_data) {
    // TODO solve
    g_print("Solve button clicked.\n");
    return;
}

// Fonction pour ajouter la boîte de contrôle à la fenêtre
GtkWidget *add_control_box(GtkWidget *box) {
    control_box = box;
    // Création du bouton "Clear"
    GtkWidget *clear_button = gtk_button_new_with_label("⨉");
    gtk_widget_set_name(clear_button, "clear-button");
    gtk_widget_set_halign(clear_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(clear_button, GTK_ALIGN_CENTER);

    // Création du bouton "Solve"
    GtkWidget *solve_button = gtk_button_new_with_label("Solve");
    gtk_widget_set_name(solve_button, "solve-button");
    gtk_widget_set_halign(solve_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(solve_button, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_end(solve_button, 40);

    // Connecter les signaux des boutons
    g_signal_connect(clear_button, "clicked",
                     G_CALLBACK(on_clear_button_clicked), NULL);
    g_signal_connect(solve_button, "clicked",
                     G_CALLBACK(on_solve_button_clicked), NULL);

    // Ajouter les boutons à la boîte
    gtk_box_pack_start(GTK_BOX(control_box), clear_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), solve_button, FALSE, FALSE, 0);

    // Retourner la boîte pour un contrôle ultérieur
    return control_box;
}

// Fonction pour afficher la boîte de contrôle
void show_control_box() {
    gtk_widget_show_all(control_box);
}

// Fonction pour masquer la boîte de contrôle
void hide_control_box() {
    gtk_widget_hide(control_box);
}
