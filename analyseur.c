#include <stdio.h>
#include <stdlib.h>

#include "fonctions.h"



int main(){
	
	
	int cmp_ligne = 1;
	FILE *f = fopen("Trame.txt", "r");
	if(f == NULL){
		printf("Le fichier 'Trame.txt' a mal été ouvert, veuillez réessayer.");
		return 0;
	}
	int curseur = 0;
	int offset_ligne = 0;
	int nb_car_ofst = nb_car_offset(f, &cmp_ligne);
	int offset = get_offset(f, nb_car_ofst, &cmp_ligne);
	Listrame * liste = total_liste(f, offset, nb_car_ofst, &cmp_ligne);
	Listrame * tmp = liste;
	while(tmp != NULL){
		printf("num : %d   ligne :%d\n", tmp -> num, tmp -> ligne);
		tmp = tmp -> suiv;
	}
	rempli_listrame(f, liste, offset, nb_car_ofst, &curseur, &offset_ligne, &cmp_ligne);
	fclose(f);
	FILE *g = fopen("Analyse.txt", "w");
	if(g == NULL){
		printf("Le fichier 'Analyse.txt' a mal été ouvert, veuillez réessayer.");
		return 0;
	}
	affiche_listrame(g, liste);
	fclose(g);
	libere_listrame(liste);

	printf("Fin sans soucis\n");
	
	
	
	return 0;
}
