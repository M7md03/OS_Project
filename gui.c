#include <gtk/gtk.h>

int Algorithm, Quanta;const char * filename;
void on_HPFb_clicked(GtkWidget *button, gpointer data)
{
    g_print(" HPF Button clicked!\n");
    Algorithm = 0;
    gtk_widget_show(GTK_WIDGET(data));
}
void on_RRb_clicked(GtkWidget *button, gpointer data)
{
    g_print(" RR Button clicked!\n");
    // The data parameter is a pointer to a GtkWidget pointer array.
    GtkWidget **widgets = (GtkWidget **)data;

    // Show the label and the combo box.
    gtk_widget_show(widgets[0]); // Label
    gtk_widget_show(widgets[1]); // Combo box
    gchar *selected_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[1]));
    while(selected_text != NULL)
        g_print("Selected: %s \n", selected_text);
    Algorithm = 2;
    gtk_widget_show(widgets[2]);
}
void on_SRTNb_clicked(GtkWidget *button, gpointer data)
{
    g_print(" SRTN Button clicked!\n");
    Algorithm = 1;
    gtk_widget_show(GTK_WIDGET(data));
}
void on_outb_clicked(GtkWidget *button, gpointer data)
{
    g_print(" submit Button clicked!\n");
    filename = gtk_entry_get_text(data);
    g_print("The file path is %s \n", filename);
}
void on_changed(GtkWidget *Combobox, gpointer data) 
{
    gchar *text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(Combobox));
    if (text != NULL) {
        g_print("Selected: %s\n", text);
        Quanta = atoi(text);
        g_free(text);
    }
    g_print("%d \t %d\n ",Algorithm, Quanta);
}
void on_RUNb_clicked(GtkButton *button, gpointer user_data) 
{
    gchar *command_line = g_strdup_printf("make run ARG=\"%d %d %s\"", Algorithm, Quanta, filename);
    GError *error = NULL;
    if (!g_spawn_command_line_async(command_line, &error)) {
        g_print("Failed to run make: %s\n", error->message);
        g_error_free(error);
    }
    g_free(command_line);
}
void on_viewb_clicked(GtkButton *button, gpointer user_data)
{
    char *args[] = {"./output.out", NULL};
    if (execv(args[0], args) == -1) {
        perror("failure in execv1");
        exit(EXIT_FAILURE);
    }
}
int main(int argc , char * argv[])
{
    gtk_init (&argc, &argv);

    GtkWidget * window;
    GtkWidget * HPFbutton;
    GtkWidget * RRbutton;
    GtkWidget * SRTNbutton;
    GtkWidget * fixed;
    GtkWidget * combobox;
    GtkWidget * Label1;
    GtkWidget * Label2;
    GtkWidget * Label3;
    GtkWidget * widgets[3];
    GtkWidget * Runbutton;
    GtkWidget * entry;
    GtkWidget * Outbutton;
    GtkWidget * Viewbutton;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
    gtk_window_set_default_size(GTK_WINDOW(window),700,500);
    
    fixed = gtk_fixed_new();
    //createng the required widgets
    HPFbutton = gtk_button_new_with_label("HPF");
    RRbutton = gtk_button_new_with_label("RR");
    SRTNbutton = gtk_button_new_with_label("SRTN");
    combobox = gtk_combo_box_text_new();
    Label1 = gtk_label_new("Choose the required Algorithm");
    Label2 = gtk_label_new("Enter the required Quanta for the RR Algorithm");
    Label3 = gtk_label_new("Enter the path file for the porcess Generator");
    Runbutton = gtk_button_new_with_label("Run the Scheduler");
    Outbutton = gtk_button_new_with_label("Submit");
    Viewbutton = gtk_button_new_with_label("generate output picture");
    entry = gtk_entry_new();

    //adding all the created widget to a fixed thing to make sure they are placed in specific positions
    gtk_fixed_put(GTK_FIXED(fixed), entry , 200, 200);
    gtk_fixed_put(GTK_FIXED(fixed), Outbutton , 400, 200);
    gtk_fixed_put(GTK_FIXED(fixed), HPFbutton, 100, 50);
    gtk_fixed_put(GTK_FIXED(fixed), RRbutton, 200, 50);
    gtk_fixed_put(GTK_FIXED(fixed), SRTNbutton, 300, 50);
    gtk_fixed_put(GTK_FIXED(fixed), combobox, 100, 120);
    gtk_fixed_put(GTK_FIXED(fixed), Label1, 100, 30);
    gtk_fixed_put(GTK_FIXED(fixed), Label2, 100, 100);
    gtk_fixed_put(GTK_FIXED(fixed), Label3, 100, 180);
    gtk_fixed_put(GTK_FIXED(fixed), Runbutton, 400, 250);
    gtk_fixed_put(GTK_FIXED(fixed), Viewbutton, 400,300);
    gtk_container_add(GTK_CONTAINER(window),fixed);
    //this widget array is to show specific widgets after the algorithms are chosen
    widgets[0] = Label2;
    widgets[1] = combobox;
    widgets[2] = Runbutton;
    //adding the values of the Quanta to combobox
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "4");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "5");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "6");
    //signals to connect any action in gui to functions
    g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(HPFbutton, "clicked", G_CALLBACK(on_HPFb_clicked), Runbutton);
    g_signal_connect(RRbutton, "clicked", G_CALLBACK(on_RRb_clicked), widgets);
    g_signal_connect(SRTNbutton, "clicked", G_CALLBACK(on_SRTNb_clicked), Runbutton);
    g_signal_connect(combobox, "changed", G_CALLBACK(on_changed), NULL);
    g_signal_connect(Runbutton, "clicked", G_CALLBACK(on_RUNb_clicked), NULL);
    g_signal_connect(Outbutton, "clicked", G_CALLBACK(on_outb_clicked), entry);
    g_signal_connect(Viewbutton, "clicked", G_CALLBACK(on_viewb_clicked), NULL);
    //show everything in window
    gtk_widget_show_all(window);

    //hide the rr values
    gtk_widget_hide(combobox);
    gtk_widget_hide(Label2);
    gtk_widget_hide(Runbutton);
    
    gtk_main();

    return 0;
}