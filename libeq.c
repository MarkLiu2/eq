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

pthread_t filterThread;
int filter_running = 0;
typedef struct{
	int start;
	int len;
	TPlayer *p;
} filterParams;
filterParams prm;

#define FILTER_M 5 //2^5 = 32
#define FILTER_KOEFS 32

int filterStart = 0;

float filterKoefs[EQ_MAX][FILTER_KOEFS];

void initFilters(int sampleRate){
	//zinicializujeme filtry
	float koefs_x[FILTER_KOEFS*2];
	float koefs_y[FILTER_KOEFS*2];
	//printf("rate: %d\n", sampleRate);
	for(int i=0; i<EQ_MAX; i++){
		//naplnime si spektrum pozadovanyma hodnotama
		float freqWindowSize = EQ_FREQUENCY_MAX/(float)(EQ_MAX+1.0);
		float freqMin = freqWindowSize*i;
		float freqMax = freqWindowSize*(i+1);
		if(i == EQ_MAX-1){ //posledni pasmo, zrusime omezeni freqMax
			freqMax = sampleRate+1.0; //pozuijeme vyssi nez vzorkovaci, tj. daleko nad rozsahem fft
		}
		//printf("min: %g, max: %g\n", freqMin, freqMax);
		for(int j=0; j<FILTER_KOEFS*2; j++){ //cele to vynulujeme
			koefs_x[j] = 0.0;
			koefs_y[j] = 0.0;
		}
		for(int j=0; j<FILTER_KOEFS; j++){
			//TODO: logaritmicke rozdeleni frekvenci
			float freq = (sampleRate/2.0)*(j/(float)FILTER_KOEFS);
			
			if(freq > freqMin && freq <= freqMax){
				koefs_x[j] = 1.0;
				koefs_x[FILTER_KOEFS*2-(j+1)] = 1.0;
			}
		}
		fft(FFT_REVERSE, FILTER_M+1, koefs_x, koefs_y);
		float norm = koefs_x[0];
		for(int j=0; j<FILTER_KOEFS*2; j++){
			//printf("x[%d]: %g\n", j, koefs_x[j]);
			koefs_x[j] /= norm; //znormalizujeme
		}
		//nahrajeme do filtru
		for(int j=0; j<FILTER_KOEFS; j++){
			filterKoefs[i][j] = koefs_x[j];
			if(j > FILTER_KOEFS/2){
				//filterKoefs[i][j] *= (1.0-j/(float)(FILTER_KOEFS/2.0));
				//filterKoefs[i][j] *= 0.0;
			}
			//filterKoefs[i][j] *= 1.0-sin(M_PI*j/(FILTER_KOEFS*2.0));
			//printf("sin[%d]: %g\n", j, 1.0-sin(M_PI*j/(FILTER_KOEFS*2.0)));
		}
	}
}

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

void doFilter(TPlayer *p, int start, int len){
	//projdeme filtry a aplikujeme
	
	start /= 2; //pracujeme se 2 byty
	len /= 2;
	printf("start: %d, am: %d, end: %d\n", start, len, start+len-1);

	for(int x=0; x<len; x++){
		float val = 0;
		for(int i=0; i<EQ_MAX; i++){
			float gain = p->eq[i];
			float curval = 0.0; //vysledna hodnota aktualniho vzorku
			//profiltrujeme
			short cursample = 0;

			for(int j=0; j<FILTER_KOEFS; j++){
				int k = start+x-j;
				cursample = ((short*)sound.data)[k];
				if(k >= 0){ //abychom necetli nekde, kde nejsou data
					curval += cursample*filterKoefs[i][j];
				}
			}
			val += curval*gain;
		}
		val /= (float)EQ_MAX; //znormalizujeme
		//printf("%*s\n", (Uint16)val, "#");
		//printf("%*s\n", ((Uint16*)sound.data)[start+x]/(256*16), "#");
		//int foo = ((short*)sound.data)[start+x];
		//printf("%d\n", foo);
		//printf("%d\n", (((Uint16*)sound.data)[start+x]-(0xFFFF/2)));
		//((short*)sound.data)[start+x] = ((short*)sound.data)[start+x]*p->eq[0];
		((short*)sound.data)[start+x] = val;
	}
	//if(len >= 1){
	//	exit(1);
	//}

	/*
	start /= 2; //pracujeme se 2 byty
	len /= 2;
	float koefs_x[FILTER_KOEFS*2];
	float koefs_y[FILTER_KOEFS*2];
	float koefs2_x[FILTER_KOEFS*2];
	float koefs2_y[FILTER_KOEFS*2];
	while(start+len > filterStart+FILTER_KOEFS){ //dokud cteme neco nefiltrovaneho
		for(int j=0; j<FILTER_KOEFS; j++){

		for(int j=0; j<FILTER_KOEFS*2; j++){ //cele to vynulujeme
			koefs_x[j] = 0.0;
			koefs_y[j] = 0.0;
		}
		for(int j=0; j<FILTER_KOEFS*2; j++){ //vlozime tam zvuk
			koefs2_x[j] = 0.0;
			koefs2_y[j] = 0.0;
		}
		//printf("rate: %d\n", sampleRate);
		for(int i=0; i<EQ_MAX; i++){
			//naplnime si spektrum pozadovanyma hodnotama
			float freqWindowSize = EQ_FREQUENCY_MAX/(float)(EQ_MAX+1.0);
			float freqMin = freqWindowSize*i;
			float freqMax = freqWindowSize*(i+1);
			if(i == EQ_MAX-1){ //posledni pasmo, zrusime omezeni freqMax
				freqMax = sampleRate+1.0; //pozuijeme vyssi nez vzorkovaci, tj. daleko nad rozsahem fft
			}
			//printf("min: %g, max: %g\n", freqMin, freqMax);
			for(int j=0; j<FILTER_KOEFS; j++){
				//TODO: logaritmicke rozdeleni frekvenci
				float freq = (sampleRate/2.0)*(j/(float)FILTER_KOEFS);

				if(freq > freqMin && freq <= freqMax){
					koefs_x[j] = p->eq[i];
					koefs_x[FILTER_KOEFS*2-(j+1)] = p->eq[i];
				}
			}
		}
		fft(FFT_REVERSE, FILTER_M+1, koefs_x, koefs_y);
		float norm = koefs_x[0];
		for(int j=0; j<FILTER_KOEFS*2; j++){
			//printf("x[%d]: %g\n", j, koefs_x[j]);
			koefs_x[j] /= norm; //znormalizujeme
		}
		//nahrajeme do filtru
		for(int j=0; j<FILTER_KOEFS; j++){
			filterKoefs[i][j] = koefs_x[j];
			if(j > FILTER_KOEFS/2){
				//filterKoefs[i][j] *= (1.0-j/(float)(FILTER_KOEFS/2.0));
				//filterKoefs[i][j] *= 0.0;
			}
			filterKoefs[i][j] *= 1.0-sin(M_PI*j/(FILTER_KOEFS*2.0));
			printf("sin[%d]: %g\n", j, 1.0-sin(M_PI*j/(FILTER_KOEFS*2.0)));
		}
	}
	*/
}
void * doFilter_thread(void *arg){
	filterParams *prm = (filterParams*)arg;
	doFilter(prm->p, prm->start, prm->len);
	pthread_exit(NULL);
	return NULL;
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
	
    SDL_MixAudio(stream, &sound.data[sound.dpos], amount, SDL_MIX_MAXVOLUME);
    sound.dpos += amount;

    amount = sound.dlen - sound.dpos;
    if (amount > (unsigned int) len)
    {
        amount = len;
    }
	
	prm.start = sound.dpos;
	prm.len = amount;
	prm.p = p;
	pthread_create(&filterThread, NULL, doFilter_thread, &prm);
	filter_running = 1;
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

	initFilters(wave.freq); //zinicializujeme audio filtry

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
