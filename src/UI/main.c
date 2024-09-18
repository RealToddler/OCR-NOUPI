#include <gtk/gtk.h>
#include <math.h>

GtkWidget *image;
GtkWidget *angle_entry;
GdkPixbuf *original_pixbuf = NULL;

static void on_file_selected(GtkWidget *widget, gpointer data)
{
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);
    char *filename = gtk_file_chooser_get_filename(chooser);

    if (filename)
    {
        original_pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (original_pixbuf != NULL)
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), original_pixbuf);
        }
        g_free(filename);
    }

    gtk_widget_destroy(GTK_WIDGET(widget));
}

static void on_open_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkWindow *parent = GTK_WINDOW(data);

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         parent,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        on_file_selected(dialog, NULL);
    }
    else
    {
        gtk_widget_destroy(dialog);
    }
}

static void on_apply_rotation(GtkWidget *widget, gpointer data)
{
    const gchar *angle_text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
    gdouble angle = g_ascii_strtod(angle_text, NULL);
    int rotation = (int)(angle / 90) * 90;
    // Apply rotation logic here
}

static void on_start_preprocessing(GtkWidget *widget, gpointer data)
{
    g_print("Début prétraitement\n");
}

static void on_start_resolution(GtkWidget *widget, gpointer data)
{
    g_print("Début résolution\n");
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Browser with Rotation and Processing");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *controls_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(controls_box, "controls_box");
    gtk_widget_set_vexpand(controls_box, TRUE);
    gtk_widget_set_valign(controls_box, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), controls_box, 0, 0, 1, 3);

    GtkWidget *open_button = gtk_button_new_with_label("Open Image");
    gtk_widget_set_size_request(open_button, 200, 50);
    gtk_box_pack_start(GTK_BOX(controls_box), open_button, FALSE, FALSE, 0);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_file), window);

    GtkWidget *hbox_angle = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(controls_box), hbox_angle, FALSE, FALSE, 0);

    angle_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(angle_entry), "Rotation angle (degrees)");
    gtk_widget_set_size_request(angle_entry, 200, 50);
    gtk_box_pack_start(GTK_BOX(hbox_angle), angle_entry, TRUE, TRUE, 0);

    GtkWidget *apply_button = gtk_button_new_with_label("Apply Rotation");
    gtk_widget_set_size_request(apply_button, 200, 50);
    gtk_box_pack_start(GTK_BOX(hbox_angle), apply_button, FALSE, FALSE, 0);
    g_signal_connect(apply_button, "clicked", G_CALLBACK(on_apply_rotation), NULL);

    GtkWidget *hbox_bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(controls_box), hbox_bottom, FALSE, FALSE, 0);

    GtkWidget *preprocess_button = gtk_button_new_with_label("Début prétraitement");
    gtk_widget_set_size_request(preprocess_button, 200, 50);
    gtk_box_pack_start(GTK_BOX(hbox_bottom), preprocess_button, TRUE, TRUE, 0);
    g_signal_connect(preprocess_button, "clicked", G_CALLBACK(on_start_preprocessing), NULL);

    GtkWidget *resolution_button = gtk_button_new_with_label("Début résolution");
    gtk_widget_set_size_request(resolution_button, 200, 50);
    gtk_box_pack_start(GTK_BOX(hbox_bottom), resolution_button, TRUE, TRUE, 0);
    g_signal_connect(resolution_button, "clicked", G_CALLBACK(on_start_resolution), NULL);

    GtkWidget *image_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(image_box, "image_box");
    gtk_widget_set_size_request(image_box, 1000, 1000);
    gtk_widget_set_hexpand(image_box, TRUE);
    gtk_widget_set_vexpand(image_box, TRUE);
    gtk_grid_attach(GTK_GRID(grid), image_box, 1, 0, 1, 3);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(image_box), image, TRUE, TRUE, 0);

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
                                    "window {"
                                    "  background-color: black;"
                                    "}"
                                    "#controls_box {"
                                    "  background-color: black;"
                                    "  padding: 20px;"
                                    "}"
                                    "#image_box {"
                                    "  background-color: white;"
                                    "  padding: 10px;"
                                    "  border: 1px solid black;"
                                    "}"
                                    "* { color: white; }",
                                    -1, NULL);

    GtkStyleContext *styleContext = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(styleContext, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.imagebrowserrotation", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}