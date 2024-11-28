#include "widget.h"
#include "NeuralNetwork/train.h"
#include "container.h"
#include "window.h"
#include <gtk/gtk.h>

// TODO REPLACE WITH THE REAL FUNCTION
#define TRAIN_NEURAL_NETWORK() (NULL)
#define ROTATE_IMAGE(path) (NULL)
#define SOLVE_IMAGE(path) (NULL)
#define PRETREATMENT_IMAGE(path) (NULL)

static GtkRevealer *control_revealer;

/**
 * @brief Copy a file
 * @param FileSource Source file path
 * @param FileDestination Destination file path
 * @return 0 if success, -1 otherwise
 */
static int file_copy(char FileSource[], char FileDestination[]) {
    char c[4096]; 
    FILE *stream_R = fopen(FileSource, "r");
    FILE *stream_W = fopen(FileDestination, "w");

    while (!feof(stream_R)) {
        size_t bytes = fread(c, 1, sizeof(c), stream_R);
        if (bytes) {
            fwrite(c, 1, bytes, stream_W);
        }
    }

    fclose(stream_R);
    fclose(stream_W);

    return 0; 
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_clear_button_clicked(GtkWidget *widget, gpointer user_data) {
    container_clear_image();
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_solve_button_clicked(GtkWidget *widget, gpointer user_data) {
    char *path = NULL;
    get_image_path(&path);
    if (path == NULL) {
        g_printerr("Erreur : aucun chemin d'image trouvé.\n");
        return;
    }
    char *solution = SOLVE_IMAGE(path);
    if (solution == NULL) {
        g_printerr("Erreur : échec de la résolution de l'image.\n");
        return;
    }
    container_set_image(get_image_container(), solution);
    free(solution);
    free(path);
    return;
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
*/
static void on_save_button_clicked(GtkWidget *widget, gpointer user_data) {
    // TODO save
    g_print("Save button clicked.\n");

    char *path = NULL;
    get_image_path(&path);

    if (path == NULL) {
        g_printerr("Erreur : aucun chemin d'image trouvé.\n");
        return;
    }

    GtkWidget *dialog;
    GtkFileChooser *chooser;

    dialog = gtk_file_chooser_dialog_new(
        "Save File", GTK_WINDOW(user_data), GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    gtk_file_chooser_set_current_name(chooser, "image.png");
    gtk_file_chooser_set_current_folder(chooser, g_get_home_dir());

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename) {
            g_print("File selected: %s\n", filename);
            file_copy(path, filename);
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);

    free(path);
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_pretreatment_button_clicked(GtkWidget *widget,
                                           gpointer user_data) {
    char *path = NULL;
    get_image_path(&path);
    if (path == NULL) {
        g_printerr("Erreur : aucun chemin d'image trouvé.\n");
        return;
    }
    char *new_path = PRETREATMENT_IMAGE(path);
    if (new_path == NULL) {
        g_printerr("Erreur : échec du prétraitement de l'image.\n");
        return;
    }
    
    container_set_image(get_image_container(), new_path);
    
    free(new_path);
    free(path);
    return;
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_rotation_button_clicked(GtkWidget *widget, gpointer user_data) {
    char * path = NULL;
    get_image_path(&path);
    if (path == NULL) {
        g_printerr("Erreur : aucun chemin d'image trouvé.\n");
        return;
    }
    char* new_path = ROTATE_IMAGE(path);
    if (new_path == NULL) {
        g_printerr("Erreur : échec de la rotation de l'image.\n");
        return;
    }
    
    container_set_image(get_image_container(), new_path);
    
    free(new_path);
    if (new_path != path)
      free(path);
    return;
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_neural_network_training_button_clicked(GtkWidget *widget,
                                                      gpointer user_data) {
    g_print("Neural network training button clicked.\n");
    int result = TRAIN_NEURAL_NETWORK();
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(user_data),
        result ? GTK_DIALOG_DESTROY_WITH_PARENT : GTK_MESSAGE_ERROR,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
        result ? "Training done." : "Training failed.");

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return;
}

GObject *init_control_box(GtkBuilder *builder) {
    GObject *revealer = gtk_builder_get_object(builder, "control-revealer");
    if (!revealer) {
        g_printerr(
            "Erreur : problème d'initialisation du révélateur de contrôle.\n");
        return NULL;
    }
    control_revealer = GTK_REVEALER(revealer);

    GObject *box = gtk_builder_get_object(builder, "control-box");
    if (!box) {
        g_printerr(
            "Erreur : problème d'initialisation de la boîte de contrôle.\n");
        return NULL;
    }

    GObject *save_button, *pretreatment_button, *rotation_button,
        *neural_network_training;
    save_button = gtk_builder_get_object(builder, "save-button");
    pretreatment_button =
        gtk_builder_get_object(builder, "pretreatment-button");
    rotation_button = gtk_builder_get_object(builder, "rotation-button");
    neural_network_training = gtk_builder_get_object(builder, "train-button");

    if (!save_button || !pretreatment_button || !rotation_button ||
        !neural_network_training) {
        g_printerr(
            "Erreur : problème d'initialisation des boutons de contrôle.\n");
        return NULL;
    }

    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked),
                     NULL);
    g_signal_connect(pretreatment_button, "clicked",
                     G_CALLBACK(on_pretreatment_button_clicked), NULL);
    g_signal_connect(rotation_button, "clicked",
                     G_CALLBACK(on_rotation_button_clicked), NULL);
    g_signal_connect(neural_network_training, "clicked",
                     G_CALLBACK(on_neural_network_training_button_clicked),
                     NULL);

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

void show_control_box() {
    gtk_revealer_set_reveal_child(control_revealer, TRUE);
}

void hide_control_box() {
    gtk_revealer_set_reveal_child(control_revealer, FALSE);
}
