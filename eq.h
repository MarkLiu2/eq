#ifndef EQ_H
#define EQ_H

#include <gtk/gtk.h>

#define EQ_MAX 8

typedef struct pParams {

    gboolean PLAY;
    gboolean PAUSE;

    float eq[EQ_MAX];

    gchar *freq; //Audio frequency in samples per second
    gchar *format; //Audio data format
    gchar *channels; //Number of channels: 1 mono, 2 stereo, 4 surround, 6 surround with center and lfe
    gchar *silence; //Audio buffer silence value (calculated)
    gchar *samples; //Audio buffer size in samples
    gchar *size; //Audio buffer size in bytes (calculated)


}TPlayer;

TPlayer player;

void play_wavfile (char* file, TPlayer *p);
#endif
