#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX_UTILISATEUR 100
#define MAX_SIZE 80
#define NAME_LIST "list_fic.csv"

enum caseFile { CREATE, UPDATE, DELETE, INEXIST};

//bool pingFile(void);

//bool lock_file(void);

/*
* prend les 2 dates et renvoie l'action a faire enum
*/
enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp);


/*
* Effectue l'action selon le enum
* switch case
*/
bool action_case_file(enum caseFile action, char* nomFichier);

/**
 * Récupère la chaine du csv et on renvoie une date
 */
time_t string_to_date(char* chaineDate);


void copy_list();

const char* getfield(char* line, int num);

 