# makefile de l'interface


all: installer msgErreur

installer: menu.o fonction.o listeChainee.o callbacks.o
	gcc -o $@ $^ -O2 `pkg-config --libs --cflags gtk+-2.0 gthread-2.0` 

msgErreur: msgErreur.o
	gcc -o $@ $^ -O2 `pkg-config --libs --cflags gtk+-2.0 gthread-2.0` 

menu.o: menu.c menu.h
	gcc -c $< `pkg-config --cflags gtk+-2.0 gthread-2.0`

fonction.o: fonction.c fonction.h menu.h
	gcc -c $< `pkg-config --cflags gtk+-2.0 gthread-2.0`

callbacks.o: callbacks.c callbacks.h menu.h
	gcc -c $< `pkg-config --cflags gtk+-2.0  gthread-2.0`

listeChainee.o: listeChainee.c listeChainee.h 
	gcc -c $< `pkg-config --cflags gtk+-2.0 gthread-2.0` 

msgErreur.o: msgErreur.c
	gcc -c $< `pkg-config --cflags gtk+-2.0 gthread-2.0` 

clean: 
	rm -f *.o installer
