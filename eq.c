#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

struct sample {
	Uint8 *data;
	Uint32 dpos;
	Uint32 dlen;
} sound;

void play(void *udata, Uint8 *stream, int len) {
	Uint32 amount = sound.dlen - sound.dpos;
	if (amount > (unsigned int) len) {
		amount = len;
	}
	SDL_MixAudio(stream, &sound.data[sound.dpos], amount, SDL_MIX_MAXVOLUME);
	sound.dpos += amount;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "!\n");
		return(EXIT_FAILURE);
	}
	if (SDL_Init(SDL_INIT_AUDIO)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = AUDIO_S16SYS;
	desired.channels = 1;
	desired.samples = 512;
	desired.callback = play;
	if (SDL_OpenAudio(&desired, NULL)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	SDL_PauseAudio(0);
	SDL_AudioSpec wave;
	Uint8 *data;
	Uint32 dlen;
	if (SDL_LoadWAV(argv[1], &wave, &data, &dlen) == NULL ) {
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_CloseAudio();
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	SDL_AudioCVT cvt;
	if (SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, desired.format, desired.channels, desired.freq) < 0) {
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_CloseAudio();
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	cvt.buf = malloc(dlen * cvt.len_mult);
	memcpy(cvt.buf, data, dlen);
	cvt.len = dlen;
	if (SDL_ConvertAudio(&cvt)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_FreeWAV(data);
		SDL_CloseAudio();
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	SDL_FreeWAV(data);
	if (sound.data) {
		free(sound.data);
	}
	SDL_LockAudio();
	sound.data = cvt.buf;
	sound.dlen = cvt.len_cvt;
	sound.dpos = 0;
	SDL_UnlockAudio();
	while (sound.dpos < sound.dlen) {
		SDL_Delay(100);
	}
	SDL_CloseAudio();
	SDL_Quit();
	return EXIT_SUCCESS;
}
