#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

#include "eq.h"

TPlayer *playerParams;

struct sample
{
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} sound;

void get_wavparams (char* file, TPlayer *p)
{
    if (SDL_Init(SDL_INIT_AUDIO))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;

    if (SDL_LoadWAV(file, &wave, &data, &dlen) == NULL )
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if(p->freq != NULL)
        g_free(p->freq);

    if(p->channels != NULL)
        g_free(p->channels);


    p->freq = g_strdup_printf("%d", wave.freq);
    switch(wave.channels) //1 mono, 2 stereo, 4 surround, 6 surround with center and lfe
    {
        case 1:
            p->channels = g_strdup_printf("%d - mono", wave.channels);
            break;

        case 2:
            p->channels = g_strdup_printf("%d - stereo", wave.channels);
            break;

        case 4:
            p->channels = g_strdup_printf("%d - surround", wave.channels);
            break;

        case 6:
            p->channels = g_strdup_printf("%d - surround with center and lfe", wave.channels);
            break;

        default:
            p->channels = g_strdup_printf("%d - undefined", wave.channels);
            break;
    }

    SDL_FreeWAV(data);

    SDL_Quit();
}

void play(void *udata, Uint8 *stream, int len)
{
    Uint32 amount = sound.dlen - sound.dpos;
    if (amount > (unsigned int) len)
    {
        amount = len;
    }

    SDL_MixAudio(stream, &sound.data[sound.dpos], amount, SDL_MIX_MAXVOLUME);
    sound.dpos += amount;
}

void play_wavfile(char* file, TPlayer *p)
{
    if (!file)
    {
        return;
    }

    playerParams = p;

    if (SDL_Init(SDL_INIT_AUDIO))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_AudioSpec desired;
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = 512;
    desired.callback = play;

    if (SDL_OpenAudio(&desired, NULL))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_PauseAudio(0);
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;

    if (SDL_LoadWAV(file, &wave, &data, &dlen) == NULL )
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_CloseAudio();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, desired.format, desired.channels, desired.freq) < 0)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_CloseAudio();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    cvt.buf = malloc(dlen * cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;

    if (SDL_ConvertAudio(&cvt))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_FreeWAV(data);
        SDL_CloseAudio();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_FreeWAV(data);

    if (sound.data)
    {
        free(sound.data);
    }

    SDL_LockAudio();
    sound.data = cvt.buf;
    sound.dlen = cvt.len_cvt;
    sound.dpos = 0;
    SDL_UnlockAudio();

    while (sound.dpos < sound.dlen)
    {
        SDL_Delay(100);

        //Somebody press STOP button
        if(!playerParams->PLAY)
            break;


        //Somebody press PAUSE button
        if(playerParams->PAUSE)
            SDL_PauseAudio(1);
        else
            SDL_PauseAudio(0);
    }


    //Neprehravam takze FALSE
    p->PLAY = FALSE;
    p->PAUSE = FALSE;

    SDL_CloseAudio();
    SDL_Quit();
}
