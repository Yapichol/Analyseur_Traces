#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"



int traduc (int c){
	int nouv;
	if(c < 60){
		nouv = c - 48;
	}
	if((c > 60) && (c < 94)){
		nouv = c - 55;
	}
	if((c > 94) && (c < 123)){
		nouv = c - 87;
	}
	return nouv;
}



void hexa_en_bin(char c, int * tab){
	int dec = traduc(c);
	int i;
	for(i = 3; i >= 0; i--){
		tab[i] = dec%2;
		dec = dec/2;
	}
	return;
}



int hexa_en_int(char * s){
	int i =0;
	int res = 0;
	int c;
	while(s[i] != '\0'){
		c = traduc(s[i]);
		res = res *16 + c;
		i++;
	}
	return res;
}



int end_of_file(FILE *f){
  int c = fgetc(f);
  ungetc(c, f);
  return (c == EOF);
}



int end_of_line(FILE *f, int curseur, int ofst){
	int c = fgetc(f);
	ungetc(c, f);
	if(curseur == ofst){
		return 0;
	}
	return (c == '\n');
}



void saute_une_ligne(FILE *f, int nb_car_ofst, int *cmp_ligne){
	int i = 0;
	while (i != '\n'){
		if(end_of_file(f) == 1){
			return;
		}
		i = fgetc(f);
	}
	*cmp_ligne = *cmp_ligne + 1;
	return;
}



int saute_ligne(FILE *f, int offset_gen, int *offset_ligne, int nb_car_ofst, int * cmp_ligne){
	*cmp_ligne = *cmp_ligne + 1;
	char ofst [nb_car_ofst + 1];
	ofst[nb_car_ofst] = '\0';
	int i = 0;
	while (i != '\n'){
		if(end_of_file(f) == 1){
			return 0;
		}
		i = fgetc(f);
	}
	for(i = 0; i < nb_car_ofst; i++){
		int c = fgetc(f);
		if(c == '\n'){
			ungetc(c, f);
			return saute_ligne(f, offset_gen, offset_ligne, nb_car_ofst, cmp_ligne);
		}
		else if(c == EOF){
			return 0;
		}
		else{
			ofst[i] = c;
		}
	}
	if((hexa_en_int(ofst) != (*offset_ligne + offset_gen)) && (hexa_en_int(ofst) != 0)){
		return saute_ligne(f, offset_gen, offset_ligne, nb_car_ofst, cmp_ligne);
	}
	for(i = 0; i < ECART; i++){
		fgetc(f);
	}
	return hexa_en_int(ofst);
}



int nb_car_offset(FILE *f, int *cmp_ligne){
	int cmp = 0;
	char i = fgetc(f);
	while(i == '0' || i == ' '){
		i = fgetc(f);
		if(i == EOF){
			return 0;
		}
		cmp++;
		if(i == ' '){
			break;
		}
	}
	if((cmp < 2) || (i != ' ')){
		while(i != '\n'){
			if(end_of_file(f) == 1){
				return 0;
			}
			i = fgetc(f);
		}
		*cmp_ligne = *cmp_ligne + 1;
		return nb_car_offset(f, cmp_ligne);
	}
	int cmp2 = 0;
	while(cmp2 != cmp){
		while((i != ' ') && (traduc(i) >= 0) && (traduc(i) <= 15)){
			i = fgetc(f);
			cmp2++;
		}
		if((cmp2 != cmp) || (i != ' ')){
			cmp2 = 0;
			while(i != '\n'){
				if(end_of_file(f) == 1){
					return 0;
				}
				i = fgetc(f);
			}
			*cmp_ligne = *cmp_ligne + 1;
		}
		i = fgetc(f);
	}
	rewind(f);
	return cmp;
}



int get_offset(FILE * f, int nb_car_ofst, int *cmp_ligne){
	int i = 0;
	int y = 0;
	while(y < (*cmp_ligne - 1)){
		if(end_of_file(f) == 1){
			return 0;
		}
		if(i == '\n'){
			y++;
		}
		i = fgetc(f);
	}
	ungetc(i, f);
	char tab[nb_car_ofst + 1];
	for(i = 0; i <= nb_car_ofst; i++){
		tab[i] = fgetc(f);
	}
	tab[nb_car_ofst] = '\0';
	rewind(f);
	printf("ligne : %d, car/ligne : %d\n", *cmp_ligne, (hexa_en_int(tab) * 3) - 1);
	*cmp_ligne = 1;
	return (hexa_en_int(tab) * 3) - 1;
}



Listrame *nouv_tram(int *cmp_ligne, int num){
	Listrame * nouv = malloc(sizeof(Listrame));
	nouv -> num = num;
	nouv -> trame = NULL;
	nouv -> ligne = *cmp_ligne;
	nouv -> err = 0;
	nouv -> suiv = NULL;
	return nouv;
}



Listrame *total_liste(FILE *f, int ofst, int nb_car_ofst, int *cmp_ligne){
	int ofst_lign = -1;
	int offset = (ofst + 1) / 3;
	Listrame * liste = NULL;
	Listrame * tmp = liste;
	int cmp = 0;
	int i;
	char tab[nb_car_ofst + 1];
	for(i = 0; i < nb_car_ofst + 1; i++){
		tab[i] = fgetc(f);
	}
	tab[nb_car_ofst] = '\0';
	if(hexa_en_int(tab) == 0){
		cmp++;
		liste = nouv_tram(cmp_ligne, cmp);
		tmp = liste;
	}
	rewind(f);
	while(end_of_file(f) != 1){
		if(saute_ligne(f, offset, &ofst_lign, nb_car_ofst, cmp_ligne) == 0){
			cmp++;
			if(liste == NULL){
				liste = nouv_tram(cmp_ligne, cmp);
				tmp = liste;
			}
			else{
				tmp -> suiv = nouv_tram(cmp_ligne, cmp);
				tmp = tmp -> suiv;
			}
		}
	}
	rewind(f);
	*cmp_ligne = 1;
	return liste;
}



void rempli_listrame(FILE *f, Listrame *liste, int ofst, int nb_car_ofst, int *curseur, int *offset_ligne, int *cmp_ligne){
	Listrame *tmp = liste;
	int i;
	int offset = (ofst + 1) / 3;
	while(tmp -> suiv != NULL){
		for(i = 1; i < tmp -> ligne; i++){
			saute_une_ligne(f, nb_car_ofst, cmp_ligne);
		}
		for(i = 0; i < nb_car_ofst + ECART; i++){
			if(end_of_file(f) == 1){
				return;
			}
			fgetc(f);
		}
		*curseur = 0;
		tmp -> trame = cree_trame(f, ofst, curseur, nb_car_ofst, offset_ligne, cmp_ligne);
		tmp -> err = erreur_trame(tmp -> trame, *cmp_ligne);
		rewind(f);
		*cmp_ligne = 1;
		*offset_ligne = 0;
		tmp = tmp -> suiv;
	}
}



void affiche_listrame(FILE *g, Listrame *liste){
	if(g == NULL){
		printf("! Le fichier a mal ete ouvert !\n");
		return;
	}
	Listrame *tmp = liste;
	while(tmp -> trame != NULL){
		fprintf(g, "TRAME %d :\n\n", tmp -> num);
		if(tmp -> err > 0){
			fprintf(g, "Attention ! Cette trame est incomplete, un probleme est survenu a la ligne %d !\n\n", tmp -> err);
		}
		if(tmp -> err == -1){
			fprintf(g, "Cette trame comporte des entetes inconnues de cet analyseur\n\n");
		}
		affiche_trame(g, tmp-> trame);
		fprintf(g, "\n\n");
		tmp = tmp -> suiv;
	}
	return;
}



void libere_listrame(Listrame *liste){
	Listrame * tmp = liste;
	while(tmp != NULL){
		liste = tmp;
		tmp = liste -> suiv;
		libere_trame(liste -> trame);
		free(liste);
	}
	liste = NULL;
	return;
}



option * nouv_option(char * type, char * taille){
	option * nouv = malloc(sizeof(option));
	nouv -> type [0] = type[0];
	nouv -> type [1] = type[1];
	nouv -> type [2] = '\0';
	nouv -> taille [0] = taille[0];
	nouv -> taille [1] = taille[1];
	nouv -> taille [2] = '\0';
	nouv -> suiv = NULL;
	return nouv;
}



void libere_option(option * opt){
	option * tmp = opt;
	while(tmp != NULL){
		opt = tmp;
		tmp = opt -> suiv;
		free(opt);
	}
	return;
}



Ethernet *lecture_ether(FILE *f, int ofst, int* curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne){
	int offset = (ofst + 1) / 3;
	Ethernet *nouv = malloc(sizeof(Ethernet));
	int i = 0;
	if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ether(nouv);
		return NULL;
	}
	for(i = 0; i<18; i++){//adresse dest
		if(*curseur != -1){
			nouv -> dest[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if((i+1)%3==0){
			nouv -> dest[i] = ':';
		}
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ether(nouv);
			return NULL;
		}
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ether(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> dest[17] = '\0';
	for(i = 0; i<18; i++){//adresse source
		if(*curseur != -1){
			nouv -> source[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if((i+1)%3 ==0){
			nouv -> source[i] = ':';
		}
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ether(nouv);
			return NULL;
		}
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ether(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> source[17] = '\0';
	for(i = 0; i<5; i++){
		if((i+1)%3 == 0){
			if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ether(nouv);
				return NULL;
			}
			if(*curseur != -1){
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ether(nouv);
			return NULL;
		}
		if(*curseur != -1){
			nouv -> type[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0 && i < 3){
				*curseur = 0;
				libere_ether(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> type[4] = '\0';
	return nouv;
}



void affiche_ether(FILE *g, Ethernet * ether){
	if(ether == NULL){
		return;
	}
	fprintf(g, "Paquet Ethernet :\n");
	fprintf(g, "	Adresse MAC destination : %s\n", ether -> dest);
	fprintf(g, "	Adresse MAC source      : %s\n", ether -> source);
	fprintf(g, "	Type : 0x%s ", ether -> type);
	if(hexa_en_int(ether -> type) == 2048){
		fprintf(g, "( IPv4 )\n");
	}
	else if(hexa_en_int(ether -> type) == 14045){
		fprintf(g, "( IPv6, pas encore pris en compte par cet analyseur)\n");
	}
	else{
		fprintf(g, "( Inconnu )\n");
	}
	return;
}



void libere_ether(Ethernet * ether){
	if(ether != NULL){
		free(ether);
	}
	return;
}



IP* lecture_ip (FILE* f, int ofst, int* curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne){
	int offset = (ofst + 1) / 3;
	int i;
	printf("ip va bien\n");
	IP* nouv =  malloc (sizeof(IP));
	if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	nouv -> type = traduc(fgetc(f));
	*curseur = *curseur + 1;         // TYPE IP
	if(nouv -> type != 4){
		return nouv;
	}
	if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	nouv -> header_length = fgetc(f);
	*curseur = *curseur + 1;         // HEADER lENGTH
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
	}
	if(*curseur != -1){
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ip(nouv);
			return NULL;
		}
		fgetc(f);
	}
	if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	*curseur = *curseur + 1;
	nouv -> tos [0] = fgetc(f);    // TOS
	*curseur = *curseur + 1;
	if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	nouv -> tos [1] = fgetc(f);
	*curseur = *curseur + 1;
	nouv -> tos [2] = '\0';
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	if(ofst == *curseur){
		*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
		if(*offset_ligne == 0){
			*curseur = 0;
			libere_ip(nouv);
			return NULL;
		}
		*curseur = -1;
	}
	if(*curseur != -1){
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ip(nouv);
			return NULL;
		}
		fgetc(f);
	}
	*curseur = *curseur + 1;
	for(i = 0; i<5; i++){      // Total Length
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_ip(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ip(nouv);
				return NULL;
			}
			nouv -> total_length[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> total_length[4] = '\0';
	for(i = 0; i<5; i++){       // Identidier
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_ip(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ip(nouv);
				return NULL;
			}
			nouv -> id[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> id[4] = '\0';
	for(i = 0; i<5; i++){       // Flags
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_ip(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ip(nouv);
				return NULL;
			}
			nouv -> flags[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	nouv -> flags[4] = '\0';
	nouv -> ttl [0] = fgetc(f);    // TTL
	*curseur = *curseur + 1;
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	nouv -> ttl [1] = fgetc(f);
	*curseur = *curseur + 1;
	nouv -> ttl [2] = '\0';
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	if(ofst == *curseur){
		*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
		if(*offset_ligne == 0){
			*curseur = 0;
			libere_ip(nouv);
			return NULL;
		}
		*curseur = -1;
	}
	if(*curseur != -1){
		if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ip(nouv);
			return NULL;
		}
		fgetc(f);
	}
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ip(nouv);
			return NULL;
	}
	*curseur = *curseur + 1;
	nouv -> protocol [0] = fgetc(f);    // Protocol
	*curseur = *curseur + 1;
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	nouv -> protocol [1] = fgetc(f);
	*curseur = *curseur + 1;
	nouv -> protocol [2] = '\0';
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_ip(nouv);
		return NULL;
	}
	if(ofst == *curseur){
		*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
		if(*offset_ligne == 0){
			*curseur = 0;
			libere_ip(nouv);
			return NULL;
		}
		*curseur = -1;
	}
	if(*curseur != -1){
		if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_ip(nouv);
			return NULL;
		}
		fgetc(f);
	}
	*curseur = *curseur + 1;
	for(i = 0; i<5; i++){       // Checksum
		if((i+1)%3 == 0){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ip(nouv);
				return NULL;
			}
			if(*curseur != -1){
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ip(nouv);
				return NULL;
			}
			nouv -> checksum[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> checksum[4] = '\0';
	for(i = 0; i<12; i++){       //adresse source
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_ip(nouv);
				return NULL;
			}
			nouv -> source[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if((i+1)%3==0){
			nouv -> source[i] = '.';
		}
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> source[11] = '\0';
	for(i = 0; i<12; i++){//adresse dest
		if(*curseur != -1){
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && i != 11){
				libere_ip(nouv);
				return NULL;
			}
			nouv -> dest[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if((i+1)%3==0){
			nouv -> dest[i] = '.';
		}
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0 && i != 11){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> dest[11] = '\0';
	if(nouv -> header_length == '5'){
		return nouv;
	}
	printf("okay\n");
	int rest = ((traduc(nouv -> header_length) * 4) - 20) * 3 ;
	char type [3];
	char taille[3];
	while(rest > 0){
		if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
			libere_ip(nouv);
			return NULL;
		}
		type[0] = fgetc(f);
		if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
			libere_ip(nouv);
			return NULL;
		}
		type[1] = fgetc(f);
		type[2] = '\0';
		*curseur = *curseur + 2;
		rest = rest - 2;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0 && rest > 1){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			*curseur = -1;
			rest = rest - 1;
		}
		if(*curseur != -1){
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
				libere_ip(nouv);
				return NULL;
			}
			fgetc(f);
			rest = rest - 1;
		}
		*curseur = *curseur + 1;
		if(hexa_en_int(type) > 1){
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
				libere_ip(nouv);
				return NULL;
			}
			taille[0] = fgetc(f);
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
				libere_ip(nouv);
				return NULL;
			}
			taille[1] = fgetc(f);
			taille[2] = '\0';
			*curseur = *curseur + 2;
			rest = rest - 2;
			if(ofst == *curseur){
				*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
				if(*offset_ligne == 0 && rest > 1){
					*curseur = 0;
					libere_ip(nouv);
					return NULL;
				}
				*curseur = -1;
				rest = rest - 1;
			}
			if(*curseur != -1){
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
					libere_ip(nouv);
					return NULL;
				}
				fgetc(f);
				rest = rest - 1;
			}
			*curseur = *curseur + 1;
		}
		option * tmp = nouv -> opt;
		if(tmp == NULL){
			nouv -> opt = nouv_option(type, taille);
		}
		else{
			while(tmp -> suiv != NULL){
				tmp = tmp -> suiv;
			}
			tmp -> suiv = nouv_option(type, taille);
		}
		if((type[0] == '0') && (type[1] == '0')){
			while(rest > 0){
				rest = rest - 1;
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
					libere_ip(nouv);
					return NULL;
				}
				fgetc(f);
				*curseur = *curseur + 1;
				if(ofst == *curseur){
					*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
					if(*offset_ligne == 0 && rest > 1){
						*curseur = 0;
						libere_ip(nouv);
						return NULL;
					}
					*curseur = 0;
					rest = rest - 1;
				}
			}
		}
		else if(hexa_en_int(type) > 1 && rest > 1){
			int longueure = (hexa_en_int(taille) * 3) - 6;
			if(longueure > rest){
				*curseur = 0;
				libere_ip(nouv);
				return NULL;
			}
			while(longueure > 0){
				rest = rest - 1;
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
					libere_ip(nouv);
					return NULL;
				}
				fgetc(f);
				longueure = longueure - 1;
				*curseur = *curseur + 1;
				if(ofst == *curseur){
					*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
					if(*offset_ligne == 0 && rest > 1){
						*curseur = 0;
						libere_ip(nouv);
						return NULL;
					}
					*curseur = 0;
					rest = rest - 1;
					longueure = longueure - 1;
				}
			}
		}
	}
	return nouv;
}



void affiche_ip(FILE *g, IP * ip){
	if(ip == NULL){
		return;
	}
	fprintf(g, "Paquet IP :\n");
	fprintf(g, "	Version : %d\n", ip -> type);
	fprintf(g, "	Header Length : 0x%c ", ip -> header_length);
	fprintf(g, "( %d octets )\n", 4 * traduc(ip -> header_length));
	fprintf(g, "	Type Of Service : 0x%s\n", ip -> tos);
	fprintf(g, "	Total Length : 0x%s ", ip -> total_length);
	fprintf(g, "( %d octets )\n", hexa_en_int(ip -> total_length));
	fprintf(g, "	Identification : 0x%s ", ip -> id);
	fprintf(g, "( %d )\n", hexa_en_int(ip -> id));
	fprintf(g, "	Drapeaux : 0x%s\n", ip -> flags);
	int floag [16];
	int o1 [4];
	hexa_en_bin(ip -> flags[0], o1);
	int o2 [4];
	hexa_en_bin(ip -> flags[1], o2);
	int o3 [4];
	hexa_en_bin(ip -> flags[2], o3);
	int o4 [4];
	hexa_en_bin(ip -> flags[4], o4);
	int i;
	for(i = 0; i < 4; i++){
		floag[i] = o1[i];
		floag[i + 4] = o2[i];
		floag[i + 8] = o3[i];
		floag[i + 12] = o4[i];
	}
	fprintf(g, "		Reserved bit   : %d...", o1[0]);
	if(o1[0]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		Don't Fragment : .%d..", o1[1]);
	if(o1[1]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		More Fragments : ..%d.", o1[2]);
	if(o1[2]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}	
	int offset = 0;
	for(i = 3; i < 16; i++){
		offset = offset * 2 + floag[i];
	}
	fprintf(g, "		Fragment Offset : %d\n", offset);
	fprintf(g, "	Time To Live : %d\n", hexa_en_int(ip -> ttl));
	int proto = hexa_en_int(ip -> protocol);
	fprintf(g, "	Protocole : 0x%s ( %d :", ip -> protocol, proto);
	if(proto == 1){
		fprintf(g, " ICMP )\n");
	}
	else if(proto == 6){
		fprintf(g, " TCP )\n");
	}
	else if(proto == 17){
		fprintf(g, " UDP )\n");
	}
	else{
		fprintf(g, " protocole inconnu)\n");
	}
	fprintf(g, "	Checksum : 0x%s\n", ip -> checksum);
	int source [4];
	int y = 0;
	for(i = 0; i < 4; i++){
		source[i] = (traduc((ip -> source[y])) * 16) + traduc(ip -> source[y + 1]);
		y = y + 3;
	}
	fprintf(g, "	Source      : %d.%d.%d.%d ( %s )\n", source[0], source[1], source[2], source[3], ip -> source);
	int dest [4];
	y = 0;
	for(i = 0; i < 4; i++){
		dest[i] = (traduc((ip -> dest[y])) * 16) + traduc(ip -> dest[y + 1]);
		y = y + 3;
	}
	fprintf(g, "	Destination : %d.%d.%d.%d ( %s )\n", dest[0], dest[1], dest[2], dest[3], ip -> dest);
	if(ip -> header_length == '5'){
		return;
	}
	fprintf(g, "	Options:\n");
	option * tmp = ip -> opt;
	while(tmp != NULL){
		int type = hexa_en_int(tmp -> type);
		fprintf(g, "		Type : %d", type);
		if(type == 0){
			fprintf(g, " ( end of options list )\n");
		}
		else if(type == 1){
			fprintf(g, " ( no operation )\n");
		}
		else if(type == 7){
			fprintf(g, " Taille : %d octets ( record route )\n", hexa_en_int(tmp -> taille));
		}
		else if(type == 68){
			fprintf(g, " Taille : %d octets ( time stamp )\n", hexa_en_int(tmp -> taille));
		}
		else if(type == 131){
			fprintf(g, " Taille : %d octets ( loose routing )\n", hexa_en_int(tmp -> taille));
		}
		else if(type == 137){
			fprintf(g, " Taille : %d octets ( strict routing )\n", hexa_en_int(tmp -> taille));
		}
		else{
			fprintf(g, " Taille : %d octets\n", hexa_en_int(tmp -> taille));
		}
		tmp = tmp -> suiv;
	}
	return;
}



void libere_ip(IP * ip){
	if(ip != NULL){
		libere_option(ip -> opt);
		free(ip);
		ip = NULL;
	}
	return;
}



TCP* lecture_tcp (FILE* f, int ofst, int* curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne){
	int offset = (ofst + 1) / 3;
	int i;
	TCP * nouv = malloc(sizeof(TCP));
	for(i = 0; i<5; i++){      // Source
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_tcp(nouv);
				return NULL;
			}
			nouv -> source[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> source[4] = '\0';
	for(i = 0; i<5; i++){      // Destination
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_tcp(nouv);
				return NULL;
			}
			nouv -> dest[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> dest[4] = '\0';
	char tab [9];
	int y = 0;
	for(i = 0; i<12; i++){       //sequence number
		if((i+1)%3==0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			y++;
		}
		else if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_tcp(nouv);
				return NULL;
			}
			tab[i - y] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	tab[8] = '\0';
	nouv -> nb_seq = hexa_en_int(tab);
	y = 0;
	for(i = 0; i<12; i++){       //acknolegement number
		if((i+1)%3==0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			y++;
		}
		else if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_tcp(nouv);
				return NULL;
			}
			tab[i - y] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	tab[8] = '\0';
	nouv -> ack_nb = hexa_en_int(tab);
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_tcp(nouv);
		return NULL;
	}
	nouv -> thl = fgetc(f);         // Total Header Length
	*curseur = *curseur + 1;
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_tcp(nouv);
		return NULL;
	}
	nouv -> flags [0] = fgetc(f);       // Flags
	*curseur = *curseur + 1;
	if(ofst == *curseur){
		*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
		if(*offset_ligne == 0){
			*curseur = 0;
			libere_tcp(nouv);
			return NULL;
		}
		*curseur = -1;
	}
	if(*curseur != -1){
		if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_tcp(nouv);
			return NULL;
		}
		fgetc(f);
	}
	*curseur = *curseur + 1;
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_tcp(nouv);
		return NULL;
	}
	nouv -> flags [1] = fgetc(f);
	*curseur = *curseur + 1;
	if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
		libere_tcp(nouv);
		return NULL;
	}
	nouv -> flags [2] = fgetc(f);
	*curseur = *curseur + 1;
	if(ofst == *curseur){
		*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
		if(*offset_ligne == 0){
			*curseur = 0;
			libere_tcp(nouv);
			return NULL;
		}
		*curseur = -1;
	}
	if(*curseur != -1){
		if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			libere_tcp(nouv);
			return NULL;
		}
		fgetc(f);
	}
	*curseur = *curseur + 1;
	for(i = 0; i<5; i++){      // Window
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_tcp(nouv);
				return NULL;
			}
			nouv -> window[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> window[4] = '\0';
	for(i = 0; i<5; i++){      // Checksum
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if (end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
				libere_tcp(nouv);
				return NULL;
			}
			nouv -> checksum[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> checksum[4] = '\0';
	for(i = 0; i<5; i++){      // Urgent Pointer
		if((i+1)%3 == 0){
			if(*curseur != -1){
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1)){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
			}
			*curseur = *curseur + 1;
		}
		if(*curseur != -1){
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && i != 4){
				libere_tcp(nouv);
				return NULL;
			}
			nouv -> urg_point[i] = fgetc(f);
		}
		*curseur = *curseur + 1;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0 && i != 4){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
		}
	}
	nouv -> urg_point[4] = '\0';
	if(nouv -> thl == '5'){
		return nouv;
	}
	int rest = ((traduc(nouv -> thl) * 4) - 20) * 3 ;
	char type [3];
	char taille[3];
	while(rest > 0){
		if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
			libere_tcp(nouv);
			return NULL;
		}
		type[0] = fgetc(f);
		if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
			libere_tcp(nouv);
			return NULL;
		}
		type[1] = fgetc(f);
		type[2] = '\0';
		*curseur = *curseur + 2;
		rest = rest - 2;
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0 && rest > 1){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			*curseur = -1;
			rest = rest - 1;
		}
		if(*curseur != -1){
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
				libere_tcp(nouv);
				return NULL;
			}
			fgetc(f);
			rest = rest - 1;
		}
		*curseur = *curseur + 1;
		if(hexa_en_int(type) > 1){
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
				libere_tcp(nouv);
				return NULL;
			}
			taille[0] = fgetc(f);
			if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
				libere_tcp(nouv);
				return NULL;
			}
			taille[1] = fgetc(f);
			taille[2] = '\0';
			*curseur = *curseur + 2;
			rest = rest - 2;
			if(ofst == *curseur){
				*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
				if(*offset_ligne == 0 && rest > 1){
					*curseur = 0;
					libere_tcp(nouv);
					return NULL;
				}
				*curseur = -1;
				rest = rest - 1;
			}
			if(*curseur != -1){
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
				rest = rest - 1;
			}
			*curseur = *curseur + 1;
		}
		option * tmp = nouv -> opt;
		if(tmp == NULL){
			nouv -> opt = nouv_option(type, taille);
		}
		else{
			while(tmp -> suiv != NULL){
				tmp = tmp -> suiv;
			}
			tmp -> suiv = nouv_option(type, taille);
		}
		if((type[0] == '0') && (type[1] == '0')){
			while(rest > 0){
				rest = rest - 1;
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
				*curseur = *curseur + 1;
				if(ofst == *curseur){
					*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
					if(*offset_ligne == 0 && rest > 1){
						*curseur = 0;
						libere_tcp(nouv);
						return NULL;
					}
					*curseur = 0;
					rest = rest - 1;
				}
			}
		}
		else if(hexa_en_int(type) > 1){
			int longueure = (hexa_en_int(taille) * 3) - 6;
			if(longueure > rest){
				*curseur = 0;
				libere_tcp(nouv);
				return NULL;
			}
			while(longueure > 0){
				rest = rest - 1;
				longueure = longueure - 1;
				if ((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && rest > 1){
					libere_tcp(nouv);
					return NULL;
				}
				fgetc(f);
				*curseur = *curseur + 1;
				if(ofst == *curseur){
					*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
					if(*offset_ligne == 0 && rest > 1){
						*curseur = 0;
						libere_tcp(nouv);
						return NULL;
					}
					*curseur = 0;
					rest = rest - 1;
					longueure = longueure - 1;
				}
			}
		}
	}
	return nouv;
}



void libere_tcp(TCP * tcp){
	if(tcp != NULL){
		libere_option(tcp -> opt);
		free(tcp);
		tcp = NULL;
	}
	return;
}



void affiche_tcp(FILE *g, TCP * tcp){
	if(tcp == NULL){
		return;
	}
	fprintf(g, "Paquet TCP :\n");
	fprintf(g, "	Port source      : %d ( 0x%s )\n", hexa_en_int(tcp -> source), tcp -> source);
	fprintf(g, "	Port destination : %d ( 0x%s )\n", hexa_en_int(tcp -> dest), tcp -> dest);
	fprintf(g, "	Numero de sequence : %u\n", tcp -> nb_seq);
	fprintf(g, "	Numero de reconnaissance : %u\n", tcp -> ack_nb);
	fprintf(g, "	Transport Header Length : 0x%c ", tcp -> thl);
	fprintf(g, "( %d octets )\n", 4 * traduc(tcp -> thl));
	fprintf(g, "	Flags : 0x%s\n", tcp -> flags);
	int o0 [4];
	hexa_en_bin(tcp -> flags[0], o0);
	int o1 [4];
	hexa_en_bin(tcp -> flags[1], o1);
	int o2 [4];
	hexa_en_bin(tcp -> flags[2], o2);
	fprintf(g, "		Reserved                : %d%d%d%d %d%d.. ....\n", o0[0], o0[1], o0[2], o0[3], o1[0], o1[1]);
	fprintf(g, "		Urg (octets urgents ?)  : .... ..%d. ....", o1[2]);
	if(o1[2]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		ACK (num reco valide ?) : .... ...%d ....", o1[3]);
	if(o1[3]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		PSH (lecture urgente ?) : .... .... %d...", o2[0]);
	if(o2[0]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		RST (reinit connexion ?): .... .... .%d..", o2[1]);
	if(o2[1]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		SYN (ouvrir connexion ?): .... .... ..%d.", o2[2]);
	if(o2[2]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "		FIN (fermer connexion ?): .... .... ...%d", o2[3]);
	if(o2[3]){
		fprintf(g, " (oui)\n");
	}
	else{
		fprintf(g, " (non)\n");
	}
	fprintf(g, "	Window : %d ( 0x%s )\n", hexa_en_int(tcp -> window), tcp -> window);
	fprintf(g, "	Checksum : 0x%s\n", tcp -> checksum);
	fprintf(g, "	Urgent pointer : %d ( 0x%s )\n", hexa_en_int(tcp -> urg_point), tcp -> urg_point);
	if(tcp -> thl == '5'){
		return;
	}
	fprintf(g, "	Options:\n");
	option * tmp = tcp -> opt;
	while(tmp != NULL){
		int type = hexa_en_int(tmp -> type);
		fprintf(g, "		Type : %d", type);
		if(type == 0){
			fprintf(g, " ( end of options list )\n");
		}
		else if(type == 1){
			fprintf(g, " ( no operation )\n");
		}
		else if(type == 2){
			fprintf(g, " Taille : %d octets ( Maximum Segment Size )\n", hexa_en_int(tmp -> taille));
		}
		else if(type == 3){
			fprintf(g, " Taille : %d octets ( Shift )\n", hexa_en_int(tmp -> taille));
		}
		else if(type == 8){
			fprintf(g, " Taille : %d octets ( Time Stamp )\n", hexa_en_int(tmp -> taille));
		}
		else{
			fprintf(g, " Taille : %d octets\n", hexa_en_int(tmp -> taille));
		}
		tmp = tmp -> suiv;
	}
	return;
}



char * lecture_http(FILE *f, int ofst, int * curseur, int tot_len_ip, int ip_len, int tcp_len, int nb_car_ofst, int *offset_ligne, int * cmp_ligne){
	int offset = (ofst + 1) / 3;
	int lign_err = 0;
	int len_http = tot_len_ip - ip_len - tcp_len;
	char * http = malloc(sizeof(char) * (len_http + 2));
	int s;
	char tab[3];
	tab[2] = '\0';
	for(s = 0; s < len_http; s++){
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			if(lign_err == 0){
				lign_err = *cmp_ligne;
			}
			libere_http(http);
			http[0] = '\0';
			//return NULL;
		}
		tab[0] = fgetc(f);
		if(end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1){
			if(lign_err == 0){
				lign_err = *cmp_ligne;
			}
			libere_http(http);
			http[0] = '\0';
			//return NULL;
		}
		tab[1] = fgetc(f);
		*curseur = *curseur + 2;
		http[s] = hexa_en_int(tab);
		if(ofst == *curseur){
			*offset_ligne = saute_ligne(f, offset, offset_ligne, nb_car_ofst, cmp_ligne);
			if(*offset_ligne == 0 && s != (len_http -1)){
				if(lign_err == 0){
					lign_err = *cmp_ligne;
				}
				*curseur = 0;
				libere_http(http);
				http[0] = '\0';
				//return NULL;
			}
			*curseur = 0;
		}
		else{
			if((end_of_file(f) == 1 || end_of_line(f, *curseur, ofst) == 1) && s != (len_http -1)){
				if(lign_err == 0){
					lign_err = *cmp_ligne;
				}
				libere_http(http);
				http[0] = '\0';
				//return NULL;
			}
			fgetc(f);
			*curseur = *curseur + 1;
		}
	}
	if(len_http == 0){
		http[0] = '\0';
		http[1] = '\0';
	}
	if(lign_err != 0){
		*cmp_ligne = lign_err;
	}
	http[len_http] = '\0';
	return http;
}



void affiche_http(FILE *g,char * http){
	if(http[0] == '\0'){
		return;
	}
	fprintf(g, "Paquet HTTP :\n\t");
	int s = 0;
	while(http[s] != '\0'){
		fprintf(g, "%c", http[s]);
		if(http[s] == '\n'){
			fprintf(g, "\t");
		}
		s++;
	}
	return;
}



void libere_http(char * http){
	/*if(http != NULL){
		free(http);
		http = NULL;
	}*/
	return;
}



Trame * cree_trame(FILE *f, int ofst, int *curseur, int nb_car_ofst, int *offset_ligne, int *cmp_ligne){
	Trame * nouv = malloc(sizeof(Trame));
	nouv -> ether = NULL;
	nouv -> ip = NULL;
	nouv -> tcp = NULL;
	nouv -> http = NULL;
	Ethernet * ether = lecture_ether(f, ofst, curseur, nb_car_ofst, offset_ligne, cmp_ligne);
	nouv -> ether = ether;
	if((ether != NULL) && (hexa_en_int(ether -> type) == 2048)){
		IP * ip = lecture_ip(f, ofst, curseur, nb_car_ofst, offset_ligne, cmp_ligne);
		printf("tout va bien\n");
		nouv -> ip = ip;
		if((ip != NULL) && (hexa_en_int(ip -> protocol)) == 6){
			TCP * tcp = lecture_tcp(f, ofst, curseur, nb_car_ofst, offset_ligne, cmp_ligne);
			nouv -> tcp = tcp;
			if((tcp != NULL) && ((hexa_en_int(tcp -> source) == 80) || (hexa_en_int(tcp -> dest) == 80))){
				char * http = lecture_http(f, ofst, curseur, hexa_en_int(ip -> total_length), traduc(ip -> header_length) * 4, traduc(tcp -> thl) * 4, nb_car_ofst, offset_ligne, cmp_ligne);
				nouv -> http = http;
			}
		}
	}
	return nouv;
}



void affiche_trame(FILE *g, Trame * trame){
	if(trame == NULL){
		fprintf(g, "Cette trame est NULLe\n");
		return;
	}
	if(trame -> ether != NULL){
		affiche_ether(g, trame -> ether);
		if(trame -> ip != NULL){
			affiche_ip(g, trame -> ip);
			if(trame -> tcp != NULL){
				affiche_tcp(g, trame -> tcp);
				if(trame -> http != NULL){
					affiche_http(g, trame -> http);
				}
			}
		}
	}
	return;
}



int erreur_trame(Trame *trame, int ligne){
	if(trame -> ether != NULL){
		if(hexa_en_int(trame -> ether -> type) == 2048){
			if(trame -> ip != NULL){
				if(hexa_en_int(trame -> ip -> protocol) == 6){
					if(trame -> tcp != NULL){
						if((hexa_en_int(trame -> tcp -> source) == 80) || (hexa_en_int(trame -> tcp -> dest) == 80)){
							if((trame -> http[0] != '\0') || (trame -> http[1] == '\0')){
								return 0;
							}
						}
						else{
							return 0;
						}
					}
				}
				else{
					return -1;
				}
			}
		}
		else{
			return -1;
		}
	}
	return ligne;
}



void libere_trame(Trame * trame){
	if(trame != NULL){/*
		libere_ether(trame -> ether);
		libere_ip(trame -> ip);
		libere_tcp(trame -> tcp);
		free(trame -> http);
		free(trame);
		trame = NULL;*/
	}
	return;
}
