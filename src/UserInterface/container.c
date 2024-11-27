#include "container.h"
#include "widget.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

// Configure le conteneur pour accepter le drag-and-drop
void container_init(GtkWidget *container, GtkWidget *label) {
    static const GtkTargetEntry targets[] = {
        {"text/uri-list", 0, 0},  // Accepter uniquement des URI (fichiers)
    };

    // Activer le drag-and-drop
    gtk_drag_dest_set(container, GTK_DEST_DEFAULT_ALL, targets,
                      G_N_ELEMENTS(targets), GDK_ACTION_COPY);

    // Connecter le signal pour recevoir des fichiers
    g_signal_connect(container, "drag-data-received",
                     G_CALLBACK(on_drag_data_received), label);
}

// Charge et affiche une image redimensionnée dans la zone centrale
void container_set_image(GtkWidget *container, const char *image_path) {
    GtkWidget *image;
    GdkPixbuf *pixbuf, *scaled_pixbuf;
    GError *error = NULL;

    // Charger l'image depuis le fichier
    pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    if (!pixbuf) {
        g_printerr("Erreur lors du chargement de l'image : %s\n", error->message);
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
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
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
}