#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <pthread.h>

#include "eq.h"

#define RANGE_MIN   0.0
#define RANGE_MAX   1.0
#define RANGE_STEP  0.1


GtkWidget *win;
GtkWidget *label_file = NULL;
GtkWidget *label_freq = NULL;
GtkWidget *label_channels = NULL;
gchar *filename;

pthread_t playerThread;


void load_fileinfo ()
{
    get_wavparams (filename, &player);


    gtk_label_set_text (GTK_LABEL(label_file), g_path_get_basename(filename));
    gtk_label_set_text (GTK_LABEL(label_freq), player.freq);
    gtk_label_set_text (GTK_LABEL(label_channels), player.channels);
}

static void
open_file ()
{
    GtkWidget *dialog;
    GtkFileFilter * fwav, * fall;

    dialog = gtk_file_chooser_dialog_new ("Open wav file...",
                                          GTK_WINDOW (win),
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);

    fwav = gtk_file_filter_new();
    gtk_file_filter_set_name(fwav,"*.wav");
    gtk_file_filter_add_pattern(fwav,"*.wav");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),fwav);

    fall = gtk_file_filter_new();
    gtk_file_filter_set_name(fall,"All");
    gtk_file_filter_add_pattern(fall,"*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),fall);


    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

        gchar *tmp = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));


        if(filename != NULL)
            g_free(filename);

        filename = g_strdup(tmp);


        g_free (tmp);

        //Defaultni nastaveni prehravace - nic neprehrava
        player.PAUSE = FALSE;
        player.PLAY = FALSE;

        load_fileinfo();

    }
    gtk_widget_destroy (dialog);
}


void *play_thread (void *p)
{

    play_wavfile (filename, p);

    pthread_exit (NULL);
    return NULL;
}


static void
change_values (GtkWidget *range, double *i)
{
    *i = gtk_range_get_value (GTK_RANGE(range));
}



static void
pause_click ()
{
    player.PAUSE = !player.PAUSE;
}


static void
stop_click ()
{
    player.PLAY = FALSE;
    player.PAUSE = FALSE;
}


static void
play_click ()
{
    if(player.PLAY && player.PAUSE)
    {
        pause_click ();
        return;
    }


    if(player.PLAY)
        return;

    if(filename != NULL)
    {

        player.PLAY = TRUE;
        player.PAUSE = FALSE;
        pthread_create (&playerThread, NULL, play_thread, &player);
    }
    else
    {
        open_file();
        if(filename != NULL)
        {

            player.PLAY = TRUE;
            player.PAUSE = FALSE;
            pthread_create (&playerThread, NULL, play_thread, &player);
        }
    }

}



int main (int argc, char *argv[])
{
    GtkWidget *vbox = NULL;
    GtkWidget *hbox = NULL;
    GtkWidget *hsep = NULL;
    GtkWidget *label = NULL;
    GtkWidget *table = NULL;
    GtkWidget *halign = NULL;

    GtkWidget *toolbar = NULL;
    GtkToolItem *open = NULL;
    //GtkToolItem *save = NULL;
    GtkToolItem *play = NULL;
    GtkToolItem *pause = NULL;
    GtkToolItem *stop = NULL;
    GtkToolItem *sep = NULL;

    /* Threads init */
    g_thread_init (NULL);
    gdk_threads_init ();

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    /* Create the main window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 0);
    gtk_window_set_title (GTK_WINDOW (win), "Pásmový ekvalizátor");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(win), 450, 300);
    gtk_widget_realize (win);
    g_signal_connect (win, "destroy", gtk_main_quit, NULL);

    /* Create a vertical box with buttons */
    vbox = gtk_vbox_new (FALSE, 6);
    gtk_container_add (GTK_CONTAINER (win), vbox);


    /* Toolbbar menu */
    toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);

    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);

    open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, -1);

//    save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
//    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, -1);

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
    g_signal_connect (G_OBJECT (pause), "clicked", G_CALLBACK (pause_click), NULL);
    g_signal_connect (G_OBJECT (stop), "clicked", G_CALLBACK (stop_click), NULL);


    hbox = gtk_hbox_new (FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 6);



    for(int i = 0; i < EQ_MAX; i++)
    {
        GtkWidget *vscale = NULL;

        vscale = gtk_vscale_new_with_range(RANGE_MIN, RANGE_MAX, RANGE_STEP);
        gtk_range_set_value  (GTK_RANGE(vscale), RANGE_MAX/2);
        gtk_range_set_inverted (GTK_RANGE(vscale), TRUE);

        g_signal_connect (G_OBJECT (vscale), "value-changed", G_CALLBACK (change_values), &(player.eq[i]));

        gtk_box_pack_start(GTK_BOX(hbox), vscale, TRUE, TRUE, 2);
    }


    hsep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox), hsep, FALSE, FALSE, 2);

    table = gtk_table_new (5, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 2);

    label = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL (label), "<span weight=\"bold\"><big>Basic file informations</big></span>");
    gtk_table_attach(GTK_TABLE(table), label, 0, 2, 0, 1,  GTK_FILL  | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 6, 2);



    halign = gtk_alignment_new(0, 0, 0, 1);
    label = gtk_label_new ("Filename:");
    gtk_container_add(GTK_CONTAINER(halign), label);
    gtk_table_attach(GTK_TABLE(table), halign, 0, 1, 1, 2,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);


    halign = gtk_alignment_new(0, 0, 0, 1);
    label_file = gtk_label_new ("-");
    gtk_container_add(GTK_CONTAINER(halign), label_file);
    gtk_table_attach(GTK_TABLE(table), halign, 1, 2, 1, 2,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);


    halign = gtk_alignment_new(0, 0, 0, 1);
    label = gtk_label_new ("Freqenncy:");
    gtk_container_add(GTK_CONTAINER(halign), label);
    gtk_table_attach(GTK_TABLE(table), halign, 0, 1, 2, 3,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);


    halign = gtk_alignment_new(0, 0, 0, 1);
    label_freq = gtk_label_new ("-");
    gtk_container_add(GTK_CONTAINER(halign), label_freq);
    gtk_table_attach(GTK_TABLE(table), halign, 1, 2, 2, 3,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);
    halign = gtk_alignment_new(0, 0, 0, 1);


    label = gtk_label_new ("Chanels:");
    gtk_container_add(GTK_CONTAINER(halign), label);
    gtk_table_attach(GTK_TABLE(table), halign, 0, 1, 3, 4,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);


    halign = gtk_alignment_new(0, 0, 0, 1);
    label_channels = gtk_label_new ("-");
    gtk_container_add(GTK_CONTAINER(halign), label_channels);
    gtk_table_attach(GTK_TABLE(table), halign, 1, 2, 3, 4,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);


//    halign = gtk_alignment_new(0, 0, 0, 1);
//    label = gtk_label_new ("Size:");
//    gtk_container_add(GTK_CONTAINER(halign), label);
//    gtk_table_attach(GTK_TABLE(table), halign, 0, 1, 4, 5,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);
//
//
//    halign = gtk_alignment_new(0, 0, 0, 1);
//    label_size = gtk_label_new ("-");
//    gtk_container_add(GTK_CONTAINER(halign), label_size);
//    gtk_table_attach(GTK_TABLE(table), halign, 1, 2, 4, 5,  GTK_FILL , GTK_FILL | GTK_EXPAND, 6, 2);


    /* Enter the main loop */
    gtk_widget_show_all (win);


    gdk_threads_enter ();

    gtk_main ();

    gdk_threads_leave ();

    return 0;
}
