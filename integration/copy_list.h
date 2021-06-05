#define MAX_UTILISATEUR 100
#define MAX_SIZE 80
#define NAME_LIST "list_fic.csv"
#define __USE_XOPEN
#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_UTILISATEUR 100
#define MAX_SIZE 80
#define MAX_PATH_SIZE 400
#define MAX_MESSAGE 500
#define NAME_LIST "list_fic.csv" /* liste des fichiers synchronises entre backup et prod */
#define NAME_PROD "production" /* serveur de production */
#define NAME_BACKUP "backup" /* serveur backup */

int recu;
int success;
int failed;

/**
 * CREATE  = Existe dans Prod pas dans BackUp
 * UPDATEB = Mettre a jour un fichier de BackUp par rapport à Prod
 * INEXIST = Existe dans BackUp pas dans Prod
 * UPDATEP = Mettre a jour un fichier dans Prod par rapport à BackUp
 */
enum caseFile {CREATE, UPDATEB, INEXIST, UPDATEP, UPTODATE};

/**
 * prend les 2 dates et renvoie l'action a faire enum
 */
enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp);

/**
 * Effectue l'action selon le enum
 * switch case
 */
bool action_case_file(enum caseFile action, char* nomFichier);

/**
 * Récupère la chaine du csv et on renvoie une date
 */
time_t string_to_date(char* chaineDate);

/**
 * Récupere la données d'une ligne csv selon un indice donné en paramètre
 * 
 */ 
const char* get_field(char* line, int num);


/**
 * Ajoute dans le fichier logs  les informations d'une copie
 * 
 */
void generate_logs(char* nomFichier, enum caseFile action, bool erreur);


void generate_stats();



void copy_list();
/*
 * transfert du fichier @param ficSrc vers la destination @param destination
 * 
 * transfert du fichier ficSrc vers la destination destination
 * @return un booleen indiquant si le transfert s'est bien effectue
 * 
 */
bool transfert(char* ficSrc,char* destination);

/**
 * Donne la taille d'une chaine de caracteres 
 * @param str chaine de caracteres a verfier
 * 
 */
int taille_char(char *str);
