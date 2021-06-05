#include <stdio.h>
#include <stdlib.h>
#include "copy_list.h"

int main(int argc, char const *argv[])
{

    printf("path : %s\n",realpath("stats.csv",NULL));
    //char t[] = "scp marwane@localhost:/mnt/d/WORK/SCHOOL/IATIC_ISTY/Annee3/PROJETS/SE_Systeme-de-Gestion-de-serveurs/integration/stats.csv /dev/null 1>/dev/null";
    //char t[] = "ls";
    transfert("stats.csv","../");

    return 0;
}

