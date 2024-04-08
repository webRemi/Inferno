#include <gtk/gtk.h>

static void print_hello(GtkWidget *widget, gpointer data);

static void activate(GtkApplication *app, gpointer user_data);

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    return status;
}

static void print_hello(GtkWidget *widget, gpointer data) {
    g_print("Hello from ASX\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *entryIp;
    GtkWidget *entryPort;
    GtkWidget *entryOperator;
    GtkWidget *textIp;
    GtkTextBuffer *bufferIp;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Inferno C2");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    grid = gtk_grid_new ();
    gtk_window_set_child (GTK_WINDOW (window), grid);

    button = gtk_button_new_with_label("Enter");
    g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
    gtk_grid_attach (GTK_GRID (grid), button, 1, 4, 1, 1);

    //image = gtk_image_new_from_file("../assets/images/Inferno.png");
    //gtk_grid_attach (GTK_GRID (grid), image, 0, 0, 2, 2);

    entryIp = gtk_entry_new();
    gtk_grid_attach(GTK_GRID (grid), entryIp, 1, 1, 1, 1);

    entryPort = gtk_entry_new();
    gtk_grid_attach(GTK_GRID (grid), entryPort, 1, 2, 1, 1);

    entryOperator = gtk_entry_new();
    gtk_grid_attach(GTK_GRID (grid), entryOperator, 1, 3, 1, 1);

    textIp = gtk_text_view_new();
    bufferIp = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textIp));
    gtk_text_buffer_set_text (bufferIp, "Hello, this is some text", -1);
    gtk_grid_attach(GTK_GRID(grid), textIp, 0, 0, 1, 1);


    gtk_window_present(GTK_WINDOW(window));
}
