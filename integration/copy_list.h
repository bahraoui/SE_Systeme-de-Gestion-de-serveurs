#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX_UTILISATEUR 100
#define MAX_SIZE 80
#define NAME_LIST "list_fic.csv"

/**
 * CREATE = Existe dans Prod pas dans BackUp
 * UPDATEB = Mettre a jour un fichier de BackUp par rapport à Prod
 * INEXIST = Existe dans BackUp pas dans Prod
 * UPDATEP = Mettre a jour un fichier dans Prod par rapport à BackUp
 */
enum caseFile {CREATE, UPDATEB, INEXIST, UPDATEP};

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
const char* getfield(char* line, int num);

void copy_list();



 