#include <stdlib.h>
#include <gtk/gtk.h>

#include "eq.h"

GtkWidget *win;
GtkWidget *label_file = NULL;

gchar *filename;


static void
open_file ()
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Open image file...",
                                          GTK_WINDOW (win),
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

        gchar *tmp = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

        const gchar* txt = g_strconcat("File: ", tmp, NULL);


        if(filename != NULL)
            g_free(filename);

        filename = g_strdup(tmp);

        gtk_label_set_text (GTK_LABEL(label_file), txt);

        g_free (tmp);
    }
    gtk_widget_destroy (dialog);
}


static void
play_click ()
{
    if(filename != NULL)
    {
        play_wavfile (filename);
    }
    else
    {
        open_file();
        if(filename != NULL)
            play_wavfile (filename);
    }
}



int main (int argc, char *argv[])
{
//    GtkWidget *button = NULL;
    GtkWidget *vbox = NULL;
//    GtkWidget *vbox2 = NULL;
    GtkWidget *hbox = NULL;
  //  GtkWidget *label = NULL;
    GtkWidget *vscale = NULL;
    GtkWidget *hsep = NULL;

    GtkWidget *toolbar = NULL;
    GtkToolItem *open = NULL;
    GtkToolItem *save = NULL;
    GtkToolItem *play = NULL;
    GtkToolItem *pause = NULL;
    GtkToolItem *stop = NULL;
    GtkToolItem *sep = NULL;

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    /* Create the main window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 0);
    gtk_window_set_title (GTK_WINDOW (win), "Pásmový ekvalizátor");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(win), 450, 200);
    gtk_widget_realize (win);
    g_signal_connect (win, "destroy", gtk_main_quit, NULL);

    /* Create a vertical box with buttons */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (win), vbox);


    /* Toolbbar menu */
    toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);

    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);

    open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, -1);

    save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, -1);

    sep = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep, -1);

    play = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), play, -1);

    pause = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PAUSE);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), pause, -1);

    stop = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop, -1);

    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);


    g_signal_connect (G_OBJECT (open), "clicked", G_CALLBACK (open_file), NULL);
    g_signal_connect (G_OBJECT (play), "clicked", G_CALLBACK (play_click), NULL);


    hbox = gtk_hbox_new (FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 6);



    // Jen pro vizualiziaci - musi se potom napsat pekne :)
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    vscale = gtk_vscale_new_with_range(0, 10, 1);
    gtk_range_set_value  (GTK_RANGE(vscale), 5);
    gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);


    hsep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox), hsep, FALSE, FALSE, 6);

    label_file = gtk_label_new ("File:");
    gtk_box_pack_start(GTK_BOX(vbox), label_file, FALSE, FALSE, 6);


    /* Enter the main loop */
    gtk_widget_show_all (win);
    gtk_main ();
    return 0;
}
