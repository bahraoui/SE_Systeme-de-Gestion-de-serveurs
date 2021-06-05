#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <stdlib.h>

void date_to_string(time_t date, char *mtime)
void list_fic(const char *chemin, FILE *file, int longPath);
void write_synchro_file(char* prod, char* backup, char* dest)
