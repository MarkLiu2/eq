#ifndef EQ_H
#define EQ_H

#include <gtk/gtk.h>

#define EQ_MAX 8

#define EQ_FREQUENCY_MAX 16000 //do jake frekvence resit ekvalizacni pasma

typedef struct pParams {

    gboolean PLAY;
    gboolean PAUSE;

    double eq[EQ_MAX]; //Hodnoty hejblatek v GUI - 0..1.0

    gchar *freq; //Audio frequency in samples per second
    gchar *channels; //Number of channels: 1 mono, 2 stereo, 4 surround, 6 surround with center and lfe

}TPlayer;

TPlayer player;

void play_wavfile (char* file, TPlayer *p);

void get_wavparams (char* file, TPlayer *p);

int getMinFrequency(int band); //vrati spodni hranicni frekvenci pro pasmo band, coz odpovida indexu posuvniku v GUI
#endif
