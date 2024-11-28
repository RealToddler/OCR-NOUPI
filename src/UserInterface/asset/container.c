#include "container.h"
#include "widget.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

static char *image_path = NULL;
static GtkWidget *image_container = NULL;

// Vérifie si le fichier est une image
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

void container_resize_image(GtkWidget *container, GdkRectangle *allocation,
                            gpointer user_data) {
    GtkWidget *image = GTK_WIDGET(user_data);

    // Vérifiez que l'image est valide
    if (!GTK_IS_WIDGET(image)) {
        g_printerr("Erreur : widget image invalide.\n");
        return;
    }

    // Obtenez les dimensions actuelles du conteneur
    int container_width = allocation->width;
    int container_height = allocation->height;

    // Chargez le pixbuf d'origine
    GdkPixbuf *original_pixbuf =
        g_object_get_data(G_OBJECT(image), "original-pixbuf");
    if (!original_pixbuf) {
        g_printerr("Erreur : Pixbuf d'origine introuvable.\n");
        return;
    }

    // Calculer les nouvelles dimensions proportionnelles
    int original_width = gdk_pixbuf_get_width(original_pixbuf);
    int original_height = gdk_pixbuf_get_height(original_pixbuf);
    double aspect_ratio = (double)original_width / (double)original_height;

    int new_width, new_height;
    if (container_width / (double)container_height > aspect_ratio) {
        new_height = container_height;
        new_width = (int)(new_height * aspect_ratio);
    } else {
        new_width = container_width;
        new_height = (int)(new_width / aspect_ratio);
    }

    // Redimensionner l'image
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(
        original_pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

    // Libérez le pixbuf redimensionné
    g_object_unref(scaled_pixbuf);
}

// Configure le conteneur pour accepter le drag-and-drop
GObject *container_init(GtkBuilder *builder, GtkWidget *window) {
    GObject *drag_drop_zone;
    GObject *drag_drop_label;

    drag_drop_zone = gtk_builder_get_object(builder, "drag-drop-zone");
    drag_drop_label = gtk_builder_get_object(builder, "drag-drop-label");

    if (!drag_drop_zone || !drag_drop_label) {
        g_printerr(
            "Erreur : problème d'initialisation du conteneur d'image.\n");
        return NULL;
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

    return drag_drop_zone;
}

void container_set_image(GtkWidget *container, const char *source_image_path) {
    image_path = malloc(strlen(source_image_path) + 1);
    if (image_path == NULL) {
        g_printerr("Erreur lors de l'allocation de la mémoire pour le chemin de "
                   "l'image.\n");
        return;
    }
    strcpy(image_path, source_image_path);

    image_container = container;
    GtkWidget *image;
    GdkPixbuf *pixbuf;
    GError *error = NULL;

    show_control_box();

    // Charger l'image depuis le fichier
    pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    if (!pixbuf) {
        g_printerr("Erreur lors du chargement de l'image : %s\n",
                   error->message);
        g_clear_error(&error);
        return;
    }

    // Créer un GtkImage et stocker le pixbuf original
    image = gtk_image_new();
    g_object_set_data(G_OBJECT(image), "original-pixbuf", pixbuf);

    // Supprimer tout contenu précédent du conteneur
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Ajouter l'image au conteneur
    gtk_container_add(GTK_CONTAINER(container), image);

    // Connecter le signal "size-allocate" à l'image
    g_signal_connect(image, "size-allocate", G_CALLBACK(container_resize_image),
                     image);

    // Afficher les modifications
    gtk_widget_show_all(container);
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

void get_image_path(char **path) {
    printf("image_path: %s\n", image_path);
    if (image_path == NULL) {
        *path = NULL;
        return;
    }
    *path = malloc(strlen(image_path) + 1);
    strcpy(*path, image_path);
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
        "Choisissez un image", GTK_WIDGET(user_data), action, "_Annuler",
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

GtkWidget *get_image_container() {
    return image_container;
}