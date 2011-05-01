#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

#include <pthread.h>

#include "libeq.h"
#include "fft.h"

TPlayer *playerParams;

struct sample
{
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} sound;
Uint8* sound_copy;

pthread_t filterThread;
int filter_running = 0;
typedef struct{
	int start;
	int len;
	TPlayer *p;
} filterParams;
filterParams prm;
Uint32 snd_freq;

#define FILTER_M 7 //2^7=128 
#define FILTER_KOEFS 128

unsigned int filterStart = 0;

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

int getMinFrequency(int band){
	int i = band;
	float mel_Max = 2595*log10(1+(EQ_FREQUENCY_MAX/700.0)); //pouzivame MEL scale
	float melWindowSize = mel_Max/(float)(EQ_MAX+1.0);
	float melMin = melWindowSize*i;
	if(i == 0){
		melMin = 0;
	}
	float freq = 700*(pow(10.0, melMin/2595.0)-1.0);
	return freq;
}

void doEqualise(TPlayer *p, unsigned int start, int len){
	start /= 2; //pracujeme se 2 byty
	len /= 2;
	float koefs_x[FILTER_KOEFS*2];
	float koefs_y[FILTER_KOEFS*2];

	//projedeme cast, kteoru budeme filtrovat
	if(len > 0){
		while(filterStart < start+len){ //dokud cteme neco nefiltrovaneho
			for(int j=0; j<FILTER_KOEFS*2; j++){ //cele to vynulujeme
				koefs_x[j] = 0.0;
				koefs_y[j] = 0.0;
			}
			int sndval = 0;
			for(int j=0; j<FILTER_KOEFS*2; j++){ //vlozime tam zvuk
				if(filterStart+j < sound.dlen/2){ //pokud jsme jeste ve zvuku
					sndval = ((short*)sound_copy)[filterStart+j];
				}else{
					sndval = 0.0;
				}
				koefs_x[j] = sndval;
			}

			fft(FFT_FORWARD, FILTER_M+1, koefs_x, koefs_y); //provedeme FFT
			for(int i=0; i<EQ_MAX; i++){
				//naplnime si spektrum pozadovanyma hodnotama
				float mel_Max = 2595*log10(1+(EQ_FREQUENCY_MAX/700.0)); //pouzivame MEL scale
				float melWindowSize = mel_Max/(float)(EQ_MAX+1.0);
				float melMin = melWindowSize*i;
				float melMax = melWindowSize*(i+1);
				if(i == EQ_MAX-1){ //posledni pasmo, zrusime omezeni melMax
					melMax = INT_MAX;
				}
				if(i == 0){
					melMin = -1;
				}
				for(int j=0; j<FILTER_KOEFS; j++){
					float freq = (snd_freq/2.0)*(j/(float)FILTER_KOEFS);
					float mel = 2595*log10(1+(freq/700.0)); //pouzivame MEL scale

					if(mel > melMin && mel <= melMax){ //vynasobime prislusnou hodnotou ekvalizeru (z gui)
						koefs_x[j] *= p->eq[i]; 
						koefs_y[j] *= p->eq[i];
						koefs_x[FILTER_KOEFS*2-(j+1)] *= p->eq[i];
						koefs_y[FILTER_KOEFS*2-(j+1)] *= p->eq[i];
					}
				}
			}
			fft(FFT_REVERSE, FILTER_M+1, koefs_x, koefs_y); //provedeme zpetnou FFT

			for(int j=0; j<FILTER_KOEFS*2; j++){ //vlozime tam zvuk
				if(filterStart+j < sound.dlen/2){ //pokud jsme jeste ve zvuku
					koefs_x[j] *= 0.5*(1-cos((2.0*M_PI*j)/(FILTER_KOEFS*2.0-1.0))); //aplikujeme hannovo okno
					((short*)sound.data)[filterStart+j] += koefs_x[j];
				}
			}
			filterStart += FILTER_KOEFS;
		}
	}
}

void play(void *udata, Uint8 *stream, int len)
{
	if(filter_running == 1){
		pthread_join(filterThread, NULL);
	}
    Uint32 amount = sound.dlen - sound.dpos;
    if (amount > (unsigned int) len)
    {
        amount = len;
    }

	TPlayer *p = udata;
	doEqualise(p, sound.dpos, amount);
	
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
	desired.userdata = p;

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

	snd_freq = wave.freq;

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
	sound_copy = malloc(dlen*cvt.len_mult);
    cvt.len = dlen;

    if (SDL_ConvertAudio(&cvt))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_FreeWAV(data);
        SDL_CloseAudio();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
	memcpy(sound_copy, cvt.buf, dlen*cvt.len_mult); //zkopirujeme
	memset(cvt.buf, 0, dlen*cvt.len_mult); //wnastavime na nuly, do nej to pak dopocitavame za behu

    SDL_FreeWAV(data);

    if (sound.data)
    {
        free(sound.data);
    }

    SDL_LockAudio();
    sound.data = cvt.buf;
    sound.dlen = cvt.len_cvt;
    sound.dpos = 0;
	filterStart = 0;
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
	free(sound_copy);
    SDL_Quit();
}
