#include "copy_list.h"

int copy_list(){
    char utilisateur[MAX_UTILISATEUR];
    cuserid(utilisateur);
    printf("utilisateur actuel : %s\n", utilisateur);
    char cmd_part1[] = "@localhost:/mnt/d/WORK/SCHOOL/IATIC_ISTY/Annee3/PROJETS/SE_Systeme-de-Gestion-de-serveurs/integration/stats.csv ";
    char cmd_part2[] = " /home/marwane/";
    //commande_ssh = malloc(taille_char(utilisateur) + sizeof(cmd_part1) + sizeof(cmd_part2));
    char* const commande_ssh[2] = {cmd_part1,cmd_part2};
    strcat(utilisateur,cmd_part1);
    printf("utilisateur +host : %s\n",cmd_part1);
    printf("commande qui va etre lance : scp %s%s\n",commande_ssh[0],commande_ssh[1]);
    if(execl("/usr/bin/scp",cmd_part1,cmd_part2,(char *)0)){
        printf("bonne fin\n");
        return EXIT_SUCCESS;
    } else {
        printf("mauvaise fin\n");
        return EXIT_FAILURE;
    }
    /*
    if(system(commande_ssh)){
        printf("bonne fin\n");
        return EXIT_SUCCESS;
    } else {
        printf("mauvaise fin\n");
        return EXIT_FAILURE;
    }*/
    return EXIT_FAILURE;
}

int taille_char(char *str){
    int i=0;
    printf("debut===\n");
    for (i = 0; i < sizeof(str); i++){
        printf("%c",str[i]);
        if (str[i]=='\0'){
            printf("\n");
            return i;
        }
    }
    printf("====fin\n");
    return 0;    
}