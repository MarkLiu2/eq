CFLAGS=-O2 -pipe -pedantic-errors -Wall -Wextra -march=native -fomit-frame-pointer -std=gnu99 `pkg-config gtk+-2.0 --cflags` `sdl-config --cflags`
LDFLAGS=`sdl-config --libs` `pkg-config gtk+-2.0 --libs`

all: eq

.PHONY: clean doc

eq: libeq.o fft.o

clean:
	$(RM) *.o README.html eq xlogin00.zip

doc:
	echo '<!DOCTYPE html><html><head><meta charset="utf-8"><title>Morphing</title></head><body>' > README.html && perl Markdown/Markdown.pl README.md >> README.html && echo '</body></html>' >> README.html

pack: doc
	zip -9 xlogin00.zip *.c *.h Makefile README.html
