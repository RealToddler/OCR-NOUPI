#include "widget.h"
#include <gtk/gtk.h>

void on_drag_data_received(GtkWidget *widget, GdkDragContext *context,
                                  gint x, gint y, GtkSelectionData *data,
                                  guint info, guint time, gpointer user_data) {
    if (gtk_selection_data_get_length(data) > 0) {
        // Récupérer les URI des fichiers déposés
        gchar **uris = gtk_selection_data_get_uris(data);
        if (uris && uris[0]) {
            gchar *filename = g_filename_from_uri(uris[0], NULL, NULL);
            if (filename) {
                g_print("Fichier déposé : %s\n", filename);

                // Mettre à jour le label avec le chemin du fichier
                GtkLabel *label = GTK_LABEL(user_data);
                gtk_label_set_text(label, filename);

                g_free(filename);
            }
        }
        g_strfreev(uris);
    } else {
        g_print("Aucune donnée reçue lors du drag-and-drop.\n");
    }

    gtk_drag_finish(context, TRUE, FALSE, time);
}


void setup_drag_and_drop(GtkWidget *event_box, GtkWidget *label) {
    static const GtkTargetEntry targets[] = {
        {"text/uri-list", 0, 0}, // Accepte les fichiers
    };

    // Active le drag-and-drop sur la zone
    gtk_drag_dest_set(event_box, GTK_DEST_DEFAULT_ALL, targets,
                      G_N_ELEMENTS(targets), GDK_ACTION_COPY);

    // Connecte le signal de réception de données
    g_signal_connect(event_box, "drag-data-received",
                     G_CALLBACK(on_drag_data_received), label);
}

void on_drag_drop_zone_clicked(GtkWidget *widget, GdkEventButton *event,
                               gpointer user_data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // Créer une boîte de dialogue pour choisir un fichier
    dialog = gtk_file_chooser_dialog_new(
        "Choisissez un fichier", GTK_WINDOW(user_data), action, "_Annuler",
        GTK_RESPONSE_CANCEL, "_Ouvrir", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    // Afficher la boîte de dialogue et gérer la réponse
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);

        // Mettre à jour le label avec le fichier sélectionné
        GtkLabel *label =
            GTK_LABEL(g_object_get_data(G_OBJECT(widget), "drag-drop-label"));
        gtk_label_set_text(label, filename);

        g_print("File selected: %s\n", filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}