#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>

int main() {
	if (SDL_Init(SDL_INIT_AUDIO)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (Mix_OpenAudio(44100, AUDIO_S16, 2, 4096)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	Mix_Chunk *sound = NULL;
	sound = Mix_LoadWAV("test.wav");
	if (sound == NULL) {
		fprintf(stderr, "%s\n", Mix_GetError());
	}
	int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
	while (Mix_Playing(channel) != 0);
	Mix_FreeChunk(sound);
	Mix_CloseAudio();
	SDL_Quit();
	return EXIT_SUCCESS;
}
