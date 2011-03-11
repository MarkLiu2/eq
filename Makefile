PROJ=eq

CC=gcc
RM=rm -f
CFLAGS=-O2 -pipe -pedantic-errors -Wall -Wextra -march=native -fomit-frame-pointer -std=c99
SDL=`sdl-config --cflags --libs` -lSDL_mixer

all: $(PROJ)

.PHONY: clean

$(PROJ): $(PROJ).c
	$(CC) $(CFLAGS) $(SDL) $< -o $@

clean:
	$(RM) $(PROJ)
