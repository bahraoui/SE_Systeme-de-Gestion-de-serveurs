#include "synchro_list.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <stdlib.h>

/*Permet de changer une date de type time_t en string*/
void date_to_string(time_t date, char *mtime){

    time_t t = date;
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(mtime, 80, "%d/%m/%Y-%T", &lt);

}


/*Permet de lire récursivement tous les dossiers et fichiers du répertoire donné en paramètre*/
void list_fic(const char *chemin, FILE *file, int longPath){

	DIR *folder;
    struct dirent *entry;
    struct stat filestat;

	//si le dossier de recherche est vide
	folder = opendir(chemin);
	if(folder == NULL)
	{
		return;
	}

	/* Lectures des fichiers et répertoires de folder */
	while( (entry=readdir(folder)) )
	{

		
		char path[1024];
		snprintf(path, sizeof(path), "%s/%s", chemin, entry->d_name);
		stat(path,&filestat);

		char mtime[80];
		//Récupération de la dernière modification du ficher analysé 
		time_t t = filestat.st_mtime;
		//transformation du type time_t en string
		date_to_string(t,mtime);


		//Si on a analysé un dossier
		if( S_ISDIR(filestat.st_mode)!=0 ){
			//abstraction des dossiers . et .. cachés
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
				continue;
			}
			
			//Pour l'affichage
			//printf("%*s[%s]\n", indent, "", entry->d_name);

			//Pour ecrire les dossiers dans le fichier
			//fprintf(file,"%s -;%s\n",path,mtime);

			//appel recursif à la fonction si on a découvert un dossier
			list_fic(path,file,longPath);
		}
		//Sinon si c'est un fichier
		else{

			//Suppression du chemin absolu
			char printPath[1024];
			strcpy(printPath,path+longPath);

			//on print le chemin du fichier et sa date de modification
			fprintf(file,"%s;%s\n",printPath,mtime);
		
			//Pour l'affichage
			//printf("%*s- %s\n", indent, "", entry->d_name);
		}
	}
	closedir(folder);

	
}


/**
*Prends en entrÃ©e deux listes de fichiers et les rassemble en un seul fichier dest.
*/
void write_synchro_file(char* prod, char* backup, char* dest, char* logs, char* stats, &mutexProd,&mutexBackUp,&mutexLogs,&mutexStats,&mutexListe) {
	//ouverture des fichiers
	pthread_mutex_lock(mutexProd);
	pthread_mutex_lock(mutexBackUp);
	pthread_mutex_lock(mutexListe);
	FILE *f1 = fopen(prod,"r");
	FILE *f2 = fopen(backup,"r");
	FILE *f3 = fopen(dest,"w");
	if (f1 == NULL) {
		return;
	}
	if (f2 == NULL) {
		return;
	}
	//init stats
	char buffStats[255];
	pthread_mutex_lock(mutexStats);
	FILE *log = fopen(stats,"a");
	fgets(buff1, 255, f1);
	char* plusP = strtok(buffStats, ";");
	char* plusB = strtok(NULL, "\n");
	fclose(stats);
	pthread_mutex_unlock(mutexStats);
	int plusProd = atoi(plusP);
	int plusBackup = atoi(plusB);
	
	//debut du traitement
	char buff1[255];
	char buff2[255];
	char buff3[512];
	buff3[0]='\0';
	int running = 1;int f1IsNull=0;int f2IsNull=0;int pass1=0;int pass2=0;
	while(running) {
		//gestion passage de tour et fin du programme
		if ((pass1 && f2IsNull) || (pass2 && f1IsNull)) {
			pass1=0;pass2=0;
		}
		if (!pass1 && !f1IsNull && fgets(buff1, 255, f1)==NULL) {
			f1IsNull=1;
		}
		if (!pass2 && !f2IsNull && fgets(buff2, 255, f2)==NULL) {
			f2IsNull=1;
		}
		if ((f1IsNull && f2IsNull)) {
			running=0;break;
		}
		pass1=0;pass2=0;
		//split
		char copybuff1[255]; char copybuff2[255];
		strcpy(copybuff1, buff1); strcpy(copybuff2, buff2);
		char* name1 = strtok(copybuff1, ";");
		char* date1 = strtok(NULL, "\n");
		char* name2 = strtok(copybuff2, ";");
		char* date2 = strtok(NULL, "\n");
		int cmp = strcmp(name1,name2);
		//comparaison
		if (cmp<0) {		//dans prod mais pas dans backup
			pass2=1;
			
			pthread_mutex_lock(mutexLogs);
			FILE *log = fopen(logs,"a");
			fprintf("fichier %s manquant dans backup\n", name1);
			plusProd++;
			fclose(log);
			pthread_mutex_unlock(mutexLogs);

			strcat(buff3,name1); strcat(buff3,";");strcat(buff3,date1); strcat(buff3,";"); strcat(buff3,"1/1/1970-01:0");strcat(buff3,"\n");
			fputs(buff3,f3);
		}
		else if (cmp>0) {		//dans backup mais pas dans prod
			pass1=1;

			pthread_mutex_lock(mutexLogs);
			FILE *log = fopen(logs,"a");
			fprintf("fichier %s manquant dans prod\n", name2);
			plusBackup++;
			fclose(log);
			pthread_mutex_unlock(mutexLogs);

			strcat(buff3,name2); strcat(buff3,";"); strcat(buff3,"1/1/1970-01:0"); strcat(buff3,";"); strcat(buff3,date2);strcat(buff3,"\n");
			fputs(buff3,f3);
		}
		else if (strcmp(name1,"|")!=0 && strcmp(name2,"|")!=0){				//les deux sont presents
			strcat(buff3,name1); strcat(buff3,";");strcat(buff3,date1); strcat(buff3,";"); strcat(buff3,date2);strcat(buff3,"\n");
			fputs(buff3,f3);
		}
		buff3[0]='\0';
	}

	pthread_mutex_lock(mutexStats);
	FILE *log = fopen(stats,"a");
	char num[12];
	sprintf(buffStats, "%d",plusProd);
	sprintf(num, "%d",plusProd);
	strcat(buffStats,";");strcat(buffStats,num)
	fclose(stats);
	pthread_mutex_unlock(mutexStats);

	fclose(f1);
	fclose(f2);
	fclose(f3);
	pthread_mutex_unlock(mutexProd);
	pthread_mutex_unlock(mutexBackUp);
	pthread_mutex_unlock(mutexListe);
}

int synchro_list(pthread_mutex_t*  mutexProd,pthread_mutex_t*  mutexBackUp,pthread_mutex_t*  mutexLogs,pthread_mutex_t*  mutexStats,pthread_mutex_t* mutexListe){

	//Ouverture du fichier de récupération de l'analyse du répertoire de production
	FILE* production = fopen("production.csv","w");

	//Ouverture du fichier de récupération de l'analyse du répertoire de production
	FILE* backUp = fopen("backUp.csv","w");

	//Chemin du répertoire à analyser du serveur de production
	char dirProd[] = ".";

	//Chemin du répertoire à analyser du serveur de backup
	char dirBack[] = ".";

	/*
	Permet de lister et de sauvegarder dans le fichier production.csv du serveur d'intégration
	la liste des fichiers contenu dans le répertoire voulu du serveur de production 
	ainsi que la date de dernière modification de chaque
	*/
	list_fic(dirProd,production,strlen(dirProd)+1);


	/*
	Permet de lister et de sauvegarder dans le fichier backUp.csv du serveur d'intégration
	la liste des fichiers contenu dans le répertoire voulu du serveur de backUp 
	ainsi que la date de dernière modification de chaque
	*/
	list_fic(dirBack,backUp,strlen(dirBack)+1);

	fprintf(production,"|;g");
	fprintf(backUp,"|;g");

	fclose(production);
	fclose(backUp);

	
	write_synchro_file("production.csv", "backUp.csv", "list_fic.csv");

   return 0;
}


