#include "widget.h"
#include "Image/Preprocess/Rotation/rotation.h"
#include "NeuralNetwork/train.h"
#include "SysCommand/syscmd.h"
#include "container.h"
#include "ocr.h"
#include "window.h"

#include <gtk/gtk.h>

#define TRAIN_NEURAL_NETWORK() train()
#define ROTATE_IMAGE(path) rotate_image_auto(path)
#define SOLVE_IMAGE(path) solve_image(path)
#define PRETREATMENT_IMAGE(path) pretreatment_image(path)

char *MAIN_IMG;
int ROTATED;

static GtkRevealer *control_revealer;

static void error_popup(const char *message) {
  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(get_window()), GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/**
 * @brief Wrapper fot the pretreatment function
 * @param path Image path
 * @return New image path
 */
static char *pretreatment_image(char *path) {
  char *new_path = NULL;
  pretreatment(path, &new_path);

  if (ROTATED == 0) {
    MAIN_IMG = strdup(path);
  }

  return new_path;
}

static char *solve_image(char *path) {
  iImage *image = load_image(path, -1);

  if (!extraction(image)) {
    error_popup("Error while extracting.");
    return NULL;
  }

  char *new_path = NULL;
  char *grid_file = get_grid_file();

  if (ROTATED == 1) {
    MAIN_IMG = strdup("resources/cache/originalRotated.png");
  }

  if (!solve(MAIN_IMG, grid_file, &new_path)) {
    error_popup("Error while solving the image.");
    return NULL;
  }
  free(grid_file);
  return new_path;
}

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

  MAIN_IMG = NULL;
  ROTATED = 0;
  sys_cmd("rm resources/cache/originalRotated.png");

  (void)widget;
  (void)user_data;
  container_clear_image();
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_solve_button_clicked(GtkWidget *widget, gpointer user_data) {
  (void)widget;
  (void)user_data;
  char *path = NULL;
  get_image_path(&path);
  if (path == NULL) {
    g_printerr("error : no path found\n");
    return;
  }
  char *solution = SOLVE_IMAGE(path);
  if (solution == NULL) {
    g_printerr("Solving failed\n");
    return;
  }
  container_set_image(get_image_container(), solution);
  free(solution);
  free(path);

  MAIN_IMG = NULL;
  ROTATED = 0;
  sys_cmd("rm resources/cache/originalRotated.png");

  return;
}

/**
 * @brief Get the image path
 * @param widget Widget
 * @param user_data User data
 */
static void on_save_button_clicked(GtkWidget *widget, gpointer user_data) {
  // TODO save
  (void)widget;
  (void)user_data;
  g_print("Save button clicked.\n");

  char *path = NULL;
  get_image_path(&path);

  if (path == NULL) {
    g_printerr("error : no path found\n");
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

  MAIN_IMG = NULL;
  ROTATED = 0;
  sys_cmd("rm resources/cache/originalRotated.png");
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
    g_printerr("error : no path found\n");
    return;
  }
  char *new_path = PRETREATMENT_IMAGE(path);
  if (new_path == NULL) {
    g_printerr("Pretreatment failed\n");
    return;
  }

  (void)widget;
  (void)user_data;

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

  ROTATED = 1;

  rotate_original_auto(MAIN_IMG);

  char *path = NULL;
  get_image_path(&path);
  if (path == NULL) {
    g_printerr("error : no path found\n");
    return;
  }

  char *new_path = ROTATE_IMAGE(path);
  if (new_path == NULL) {
    g_printerr("Rotation failed\n");
    free(path);
    return;
  }

  container_set_image(get_image_container(), new_path);

  if (new_path != path) {
    free(new_path);
  }

  (void)widget;
  (void)user_data;

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
  int result = train();
  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(user_data), GTK_DIALOG_DESTROY_WITH_PARENT,
      result ? GTK_MESSAGE_INFO : GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s",
      result ? "Training done." : "Training failed.");

  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

  (void)widget;

  return;
}

GObject *init_control_box(GtkBuilder *builder) {
  GObject *revealer = gtk_builder_get_object(builder, "control-revealer");
  if (!revealer) {
    g_printerr("error\n");
    return NULL;
  }
  control_revealer = GTK_REVEALER(revealer);

  GObject *box = gtk_builder_get_object(builder, "control-box");
  if (!box) {
    g_printerr("error\n");
    return NULL;
  }

  GObject *save_button, *pretreatment_button, *rotation_button,
      *neural_network_training;
  save_button = gtk_builder_get_object(builder, "save-button");
  pretreatment_button = gtk_builder_get_object(builder, "pretreatment-button");
  rotation_button = gtk_builder_get_object(builder, "rotation-button");
  neural_network_training = gtk_builder_get_object(builder, "train-button");

  if (!save_button || !pretreatment_button || !rotation_button ||
      !neural_network_training) {
    g_printerr("error\n");
    return NULL;
  }

  g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked),
                   NULL);
  g_signal_connect(pretreatment_button, "clicked",
                   G_CALLBACK(on_pretreatment_button_clicked), NULL);
  g_signal_connect(rotation_button, "clicked",
                   G_CALLBACK(on_rotation_button_clicked), NULL);
  g_signal_connect(neural_network_training, "clicked",
                   G_CALLBACK(on_neural_network_training_button_clicked), NULL);

  GtkWidget *control_box = GTK_WIDGET(box);

  GtkWidget *clear_button = gtk_button_new_with_label("⨉");
  gtk_widget_set_name(clear_button, "clear-button");
  gtk_widget_set_halign(clear_button, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(clear_button, GTK_ALIGN_CENTER);

  GtkWidget *solve_button = gtk_button_new_with_label("Solve");
  gtk_widget_set_name(solve_button, "solve-button");
  gtk_widget_set_halign(solve_button, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(solve_button, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_end(solve_button, 40);

  g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked),
                   NULL);
  g_signal_connect(solve_button, "clicked", G_CALLBACK(on_solve_button_clicked),
                   NULL);

  gtk_box_pack_start(GTK_BOX(control_box), clear_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(control_box), solve_button, FALSE, FALSE, 0);

  return revealer;
}

void show_control_box() {
  gtk_revealer_set_reveal_child(control_revealer, TRUE);
}

void hide_control_box() {
  gtk_revealer_set_reveal_child(control_revealer, FALSE);
}
