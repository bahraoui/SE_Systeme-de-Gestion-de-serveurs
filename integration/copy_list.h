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
#include <pthread.h>

#define MAX_UTILISATEUR 100
#define MAX_SIZE 80
#define MAX_PATH_SIZE 400
#define MAX_MESSAGE 100
#define NAME_LIST "list_fic.csv" /* liste des fichiers synchronises entre backup et prod */
#define NAME_PROD "../production/" /* serveur de production */
#define NAME_BACKUP "../backup/" /* serveur backup */

/**
 * Le nombre de fichier recu par le module
 */
int recu;
/**
 * Le nombre de mofication reussi
 */
int success;
/**
 * Le nombre de mofication echoué
 */
int failed;

/**
 * CREATE  = Existe dans Prod pas dans BackUp
 * UPDATEB = Mettre a jour un fichier de BackUp par rapport à Prod
 * INEXIST = Existe dans BackUp pas dans Prod
 * UPDATEP = Mettre a jour un fichier dans Prod par rapport à BackUp
 */
enum caseFile {CREATE, UPDATEB, INEXIST, UPDATEP, UPTODATE};

/**
 * Cette fonction prend les dates en paramètre et renvoie l'action en conséquence
 * @param dateProd la date de modification dans le serveur Production
 * @param dateBackUp la date de modification dans le serveur BackUp
 */
enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp);

/**
 * Effectue l'action de copie selon le paramètre @param action 
 * @param nomFichier le nom du fichier a que l'on doit créer/copier 
 * La fonction crée ou met à jour les fichiers et incrémente le compteur de réussite et d'echecs
 */
bool action_case_file(enum caseFile action, char* nomFichier);

/**
 * Récupère une chaine et la transforme en date
 * @param chaineDate la chaine a transformé
 * @return la date renvoyé dans le bon type
 */
time_t string_to_date(char* chaineDate);

/**
 * Récupere le contenu d'une ligne csv selon un indice donné en paramètre
 * @param line la line du csv que l'on parcours, @param num l'indice de l'élément dans la ligne
 * 
 */ 
const char* get_field(char* line, int num);

/**
 * Ajoute dans le fichier logs les informations d'une copie
 * Les informations de cette copie sont : @param nomFichier le nom du fichier a copié/crée,
 * @param action l'action a effectué sur le fichier, @param erreur le resultat de l'action effectué sur le fichier
 * 
 */
void generate_logs(char* nomFichier, enum caseFile action, bool erreur);

/**
 * Ajoute dans le fichier stats les informations de toute la phase de copie
 * 
 */
void generate_stats();


/**
 * Fonction principale du module CopyList
 * 
 */
void copy_list(pthread_mutex_t*  mutexProd,pthread_mutex_t*  mutexBackUp,pthread_mutex_t*  mutexLogs,pthread_mutex_t*  mutexStats,pthread_mutex_t* mutexListe);


/**
 * transfert du fichier @param ficSrc vers la destination @param destination
 * 
 * transfert du fichier ficSrc vers la destination destination
 * @return un booleen indiquant si le transfert s'est bien effectue
 * 
 */
bool transfert(char* ficSrc,char* destination);

bool test_server(char* ficSrc,char* destination);

/**
 * Donne la taille d'une chaine de caracteres 
 * @param str chaine de caracteres a verfier
 * 
 */
int taille_char(char *str);
