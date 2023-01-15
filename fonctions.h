#ifndef FONCTIONS
#define FONCTIONS
#define ECART 3



typedef struct ethernet{
	char dest[18];
	char source[18];
	char type[5];
}Ethernet;


typedef struct Option{
	char type [3];
	char taille [3];
	struct Option * suiv;
}option;


typedef struct ip{
	int type;
	char header_length;
	char tos [3]; 
	char total_length [5];
	char id [5];
	char flags [5];
	char ttl [3];
	char protocol [3];
	char checksum [5];
	char source [12];
	char dest [12];
	option * opt;
}IP;


typedef struct tcp{
	char source [5];
	char dest [5];
	unsigned int nb_seq;
	unsigned int ack_nb;
	char thl;
	char flags [4];
	char window [5];
	char checksum [5];
	char urg_point [5];
	option * opt;
}TCP;


typedef struct trame{
	Ethernet * ether;
	IP * ip;
	TCP * tcp;
	char * http;
}Trame;


typedef struct listrame{
	int num;
	Trame * trame;
	int ligne;
	int err;
	struct listrame * suiv;
}Listrame;


int traduc (int c);

void hexa_en_bin(char c, int * tab);

int hexa_en_int(char * s);

int end_of_file(FILE *f);

int end_of_line(FILE *f, int curseur, int ofst);

void saute_une_ligne(FILE *f, int nb_car_ofst, int *cmp_ligne);

int saute_ligne(FILE *f, int offset_gen, int *offset_ligne, int nb_car_ofst, int * cmp_ligne);

int nb_car_offset(FILE *f, int *cmp_ligne);

int get_offset(FILE * f, int nb_car_ofst, int *cmp_ligne);

Listrame *nouv_tram(int *cmp_ligne, int num);

Listrame *total_liste(FILE *f, int ofst, int nb_car_ofst, int *cmp_ligne);

void rempli_listrame(FILE *f, Listrame *liste, int ofst, int nb_car_ofst, int *curseur, int *offset_ligne, int *cmp_ligne);

void affiche_listrame(FILE *g, Listrame *liste);

void libere_listrame(Listrame * liste);

option * nouv_option(char * type, char * taille);

void libere_option(option * opt);

// ETHERNET
Ethernet *lecture_ether(FILE *f, int ofst, int* curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne);

void affiche_ether(FILE *g, Ethernet * ether);

void libere_ether(Ethernet * ether);

// IP
IP* lecture_ip (FILE* f, int ofst, int* curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne);

void affiche_ip(FILE *g, IP * ip);

void libere_ip(IP * ip);

// TCP
TCP* lecture_tcp (FILE* f, int ofst, int* curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne);

void libere_tcp(TCP * tcp);

void affiche_tcp(FILE *g, TCP * tcp);

// HTTP
char * lecture_http(FILE *f, int ofst, int * curseur, int tot_len_ip, int ip_len, int tcp_len, int nb_car_ofst, int *offset_ligne, int * cmp_ligne);

void affiche_http(FILE *g, char * http);

void libere_http(char * http);

//TRAME
Trame * cree_trame(FILE *f, int ofst, int *curseur, int nb_car_ofst, int *offset_ligne, int * cmp_ligne);

void affiche_trame(FILE *g, Trame * trame);

int erreur_trame(Trame *trame, int ligne);

void libere_trame(Trame * trame);



#endif
