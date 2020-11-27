CFLAGS = -Wall -Wextra -pedantic -lX11 -lXft -I/usr/include/freetype2 -pthread

PREFIX ?= /usr/local
CC ?= cc

all: clean herbe

config.h: config.def.h
	cp config.def.h config.h

herbe: herbe.c config.h
	$(CC) herbe.c $(CFLAGS) -o herbe

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f herbe ${DESTDIR}${PREFIX}/bin

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/herbe

clean:
	rm -f herbe config.h

.PHONY: all install uninstall clean
