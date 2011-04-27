CFLAGS=-O2 -pipe -pedantic-errors -Wall -Wextra -march=native -fomit-frame-pointer -std=gnu99 `pkg-config gtk+-2.0 --cflags` `sdl-config --cflags`
LDFLAGS=`sdl-config --libs` `pkg-config gtk+-2.0 --libs`

all: eq

.PHONY: clean

eq: libeq.o fft.o

clean:
	$(RM) *.o eq
