PROJ=eq

CC=gcc
RM=rm -f
CFLAGS=-O2 -pipe -pedantic-errors -Wall -Wextra -march=native -fomit-frame-pointer -std=gnu99
GTK=`pkg-config gtk+-2.0 --cflags --libs`
SDL=`sdl-config --cflags --libs`

OBJS=eq.o

all: $(PROJ)

.PHONY: clean

%.o: %.c
	$(CC) $(CFLAGS) $(SDL) $(GTK) -c $<

$(PROJ): $(OBJS) gui.c
	$(CC) $(CFLAGS) $(OBJS) $(SDL) $(GTK) gui.c -o $(PROJ)

clean:
	$(RM) *.o $(PROJ)
