#include "container.h"
#include "widget.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

static GtkWidget *image_container = NULL;

static int is_image_file(const char *filename) {
    const char *extensions[] = {".png"};
    for (unsigned long i = 0; i < sizeof(extensions) / sizeof(extensions[0]);
         i++) {
        if (g_str_has_suffix(filename, extensions[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

// Configure le conteneur pour accepter le drag-and-drop
int container_init(GtkBuilder *builder, GtkWidget *window) {
    GObject *drag_drop_zone;
    GObject *drag_drop_label;

    drag_drop_zone = gtk_builder_get_object(builder, "drag-drop-zone");
    drag_drop_label = gtk_builder_get_object(builder, "drag-drop-label");

    if (!drag_drop_zone || !drag_drop_label) {
        g_printerr(
            "Erreur : problème d'initialisation du conteneur d'image.\n");
        return FALSE;
    }

    GtkWidget *container = GTK_WIDGET(drag_drop_zone);
    GtkWidget *label = GTK_WIDGET(drag_drop_label);
    static const GtkTargetEntry targets[] = {
        {"text/uri-list", 0, 0}, // Accepter uniquement des URI (fichiers)
    };

    // Activer le drag-and-drop
    gtk_drag_dest_set(container, GTK_DEST_DEFAULT_ALL, targets,
                      G_N_ELEMENTS(targets), GDK_ACTION_COPY);

    g_signal_connect(GTK_WIDGET(drag_drop_zone), "button-press-event",
                     G_CALLBACK(on_drag_drop_zone_clicked), window);

    g_object_set_data(G_OBJECT(drag_drop_zone), "drag-drop-label",
                      drag_drop_label);

    // Connecter le signal pour recevoir des fichiers
    g_signal_connect(container, "drag-data-received",
                     G_CALLBACK(on_drag_data_received), label);

    return TRUE;
}

// Charge et affiche une image redimensionnée dans la zone centrale
void container_set_image(GtkWidget *container, const char *image_path) {
    image_container = container;

    GtkWidget *image;
    GdkPixbuf *pixbuf, *scaled_pixbuf;
    GError *error = NULL;

    // Charger l'image depuis le fichier
    pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    if (!pixbuf) {
        g_printerr("Erreur lors du chargement de l'image : %s\n",
                   error->message);
        g_clear_error(&error);
        return;
    }

    // Obtenir les dimensions actuelles de la zone centrale
    int container_width = gtk_widget_get_allocated_width(container);
    int container_height = gtk_widget_get_allocated_height(container);

    // Si les dimensions sont invalides, utiliser des valeurs par défaut
    if (container_width == 0 || container_height == 0) {
        container_width = 400;  // Largeur par défaut
        container_height = 300; // Hauteur par défaut
    }

    // Calculer les dimensions proportionnelles pour l'image
    int original_width = gdk_pixbuf_get_width(pixbuf);
    int original_height = gdk_pixbuf_get_height(pixbuf);
    double aspect_ratio = (double)original_width / (double)original_height;

    int new_width, new_height;
    if (container_width / (double)container_height > aspect_ratio) {
        new_height = container_height;
        new_width = (int)(new_height * aspect_ratio);
    } else {
        new_width = container_width;
        new_height = (int)(new_width / aspect_ratio);
    }

    // Redimensionner l'image proportionnellement
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height,
                                            GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf); // Libérer le pixbuf d'origine

    // Créer un GtkImage à partir du pixbuf redimensionné
    image = gtk_image_new_from_pixbuf(scaled_pixbuf);
    g_object_unref(scaled_pixbuf); // Libérer le pixbuf redimensionné

    // Supprimer tout contenu précédent du conteneur
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Ajouter l'image au conteneur
    gtk_container_add(GTK_CONTAINER(container), image);

    // Afficher les modifications
    gtk_widget_show_all(container);
    show_control_box();
}

// Fonction pour clear l'image
void container_clear_image() {
    if (image_container == NULL)
        return;
    // Supprimer tout contenu précédent du conteneur
    GList *children =
        gtk_container_get_children(GTK_CONTAINER(image_container));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Ajouter un texte par défaut pour le drag-and-drop
    GtkWidget *label =
        gtk_label_new("Cliquez ou glissez-déposez un fichier ici");
    gtk_widget_set_name(
        label, "drag-drop-label"); // Assurez-vous que le style correspond
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

    // Ajouter le texte au conteneur
    gtk_container_add(GTK_CONTAINER(image_container), label);
    gtk_widget_show_all(image_container);

    hide_control_box();
    image_container = NULL;
}

void on_drag_data_received(GtkWidget *widget, GdkDragContext *context,
                           __attribute__((unused)) gint x,
                           __attribute__((unused)) gint y,
                           GtkSelectionData *data,
                           __attribute__((unused)) guint info, guint time,
                           __attribute__((unused)) gpointer user_data) {
    if (gtk_selection_data_get_length(data) > 0) {
        // Récupérer les URI des fichiers déposés
        gchar **uris = gtk_selection_data_get_uris(data);
        if (uris && uris[0]) {
            gchar *filename = g_filename_from_uri(uris[0], NULL, NULL);
            if (filename) {
                if (!is_image_file(filename)) {
                    g_printerr("Le fichier déposé n'est pas une image.\n");
                } else {
                    g_print("Fichier déposé : %s\n", filename);

                    // Mettre à jour le label avec le chemin du fichier
                    // GtkLabel *label = GTK_LABEL(user_data);
                    // gtk_label_set_text(label, filename);

                    container_set_image(widget, filename);
                }
                g_free(filename);
            }
        }
        g_strfreev(uris);
    } else {
        g_print("Aucune donnée reçue lors du drag-and-drop.\n");
    }

    gtk_drag_finish(context, TRUE, FALSE, time);
}

void on_drag_drop_zone_clicked(GtkWidget *widget,
                               __attribute__((unused)) GdkEventButton *event,
                               gpointer user_data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // Créer une boîte de dialogue pour choisir un fichier
    dialog = gtk_file_chooser_dialog_new(
        "Choisissez un image", GTK_WINDOW(user_data), action, "_Annuler",
        GTK_RESPONSE_CANCEL, "_Ouvrir", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    // Afficher la boîte de dialogue et gérer la réponse
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);

        if (!is_image_file(filename)) {
            g_printerr("Le fichier sélectionné n'est pas une image.\n");
        } else {
            g_print("File selected: %s\n", filename);
            container_set_image(widget, filename);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}
