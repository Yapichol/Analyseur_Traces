all: analyseur

fonctions.o: fonctions.c fonctions.h
	gcc -c fonctions.c -o fonctions.o
	
analyseur.o: analyseur.c
	gcc -c analyseur.c

analyseur: fonctions.o analyseur.o
	gcc -o analyseur fonctions.o analyseur.o
