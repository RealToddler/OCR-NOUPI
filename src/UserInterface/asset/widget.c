#include "widget.h"
#include "container.h"
#include "window.h"
#include <gtk/gtk.h>

static GtkRevealer *control_revealer;

static void on_clear_button_clicked(GtkWidget *widget, gpointer user_data) {
    container_clear_image();
}

static void on_solve_button_clicked(GtkWidget *widget, gpointer user_data) {
    // TODO solve
    g_print("Solve button clicked.\n");
    return;
}

// Fonction pour ajouter la boîte de contrôle à la fenêtre
GObject *init_control_box(GtkBuilder *builder) {
    GObject *revealer = gtk_builder_get_object(builder, "control-revealer");
    if (!revealer) {
        g_printerr(
            "Erreur : problème d'initialisation du révélateur de contrôle.\n");
        return FALSE;
    }
    control_revealer = GTK_REVEALER(revealer);

    GObject *box = gtk_builder_get_object(builder, "control-box");
    if (!box) {
        g_printerr(
            "Erreur : problème d'initialisation de la boîte de contrôle.\n");
        return FALSE;
    }

    GtkWidget *control_box = GTK_WIDGET(box);

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
    return revealer;
}

// Fonction pour afficher la boîte de contrôle
void show_control_box() {
    gtk_revealer_set_reveal_child(control_revealer, TRUE);
}

// Fonction pour masquer la boîte de contrôle
void hide_control_box() {
    gtk_revealer_set_reveal_child(control_revealer, FALSE);
}