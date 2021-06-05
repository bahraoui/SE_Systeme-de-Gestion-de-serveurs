#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <stdlib.h>

/**
*Permet de transformer une date de type time_t en string afin de l'écrire dans un fichier csv
*@param date La date à convertir
*@param La chaine de caractère dans laquelle nous allons stocker le résultat de la conversion
*/
void date_to_string(time_t date, char *mtime);

/*
*Permet de lister les fichiers d'un répertoire donné de manière récursive 
*@param chemin le chemin du dossier que l'on souhaite analyser
*@param file Le fichier dans lequel on souhaite stocker le chemin de ce fichier et sa dernière date de modification
*@param longPath La longueur du chemin du répertoire initial à analyser afin d'enlever le morceau de chemin de chaque ligne à écrire dans file
*/
void list_fic(const char *chemin, FILE *file, int longPath);

/*
*Permet de récupérer la liste des fichiers d'un répertoire donné (choisi lors de l'appel à list_fic()), maintenue dans le fichier du nom prod et du nom backup. 
*On compare ensuite 1 à 1 les fichiers de prod et de backup afin d'écrire dans le fichier du nom dest, le chemin du fichier ainsi que sa date de dernière modification sur le serveur production puis backup
*@param prod nom du fichier de la liste des fichiers du serveur production
*@param backup nom du fichier de la liste des fichiers du serveur backup
*@param dest nom du fichier dans lequel opn va rassembler les données
*@param mutexProd mutex sur le serveur production
*@param mutexBackUp mutex sur le serveur backup
*@param mutexLogs mutex sur le fichier des logs
*@param mutexStats mutex sur le fichier des stats de sync_list
*@param mutexListe mutex sur le fichiers qui liste les fichiers des deux serveurs
*/
void write_synchro_file(char* prod, char* backup, char* dest, char* logs, char* stats, pthread_mutex_t*  mutexProd,pthread_mutex_t*  mutexBackUp,pthread_mutex_t*  mutexLogs,pthread_mutex_t*  mutexStats,pthread_mutex_t* mutexListe);

/*
*Permet de lancer le module synchro list produisant un fichier des noms de fichiers du serveur Production et backUp ainsi que leurs dates de modification au sein de ses deux serveurs
*@param mutexProd mutex sur le serveur production
*@param mutexBackUp mutex sur le serveur backup
*@param mutexLogs mutex sur le fichier des logs
*@param mutexStats mutex sur le fichier des stats de sync_list
*@param mutexListe mutex sur le fichiers qui liste les fichiers des deux serveurs
*@return Un entier comme quoi la synchronisation c'est bien passée
*/
int synchro_list(pthread_mutex_t*  mutexProd,pthread_mutex_t*  mutexBackUp,pthread_mutex_t*  mutexLogs,pthread_mutex_t*  mutexStats,pthread_mutex_t* mutexListe);
