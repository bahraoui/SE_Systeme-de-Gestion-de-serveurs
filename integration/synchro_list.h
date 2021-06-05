#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

void date_to_string(time_t date, char *mtime);
void list_fic(const char *chemin, FILE *file, int longPath);
void write_synchro_file(char* prod, char* backup, char* dest);
int synchro_list(pthread_mutex_t*  mutexProd,pthread_mutex_t*  mutexBackUp,pthread_mutex_t*  mutexLogs,pthread_mutex_t*  mutexStats,pthread_mutex_t* mutexListe);
