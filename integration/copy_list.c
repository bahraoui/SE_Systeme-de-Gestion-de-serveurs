#include "copy_list.h"

void copy_list()
{

    //A FAIRE DANS LE MAIN

    /*if (false){
        printf("PING PAS PASSE");
        exit(EXIT_FAILURE);
    }

    if (!lock_file()) { //lock list_fic.csv
        printf("PAS REUSSI A VERROUILLER");
        exit(EXIT_FAILURE);
    }*/

    //parcours chaque ligne du CSV
    char ligne[1024];
    enum caseFile action;
    time_t dateProd;
    time_t dateBackUp;

    FILE *listCSV = fopen(NAME_LIST, "r");

    if (listCSV == NULL)
    {
        printf("Cannot open file %s\n", NAME_LIST);
        exit(0);
    }
    fgets(ligne, 1024, listCSV);
    while (fgets(ligne, 1024, listCSV))
    {
        char *nomFichier = strdup(getfield(strdup(ligne), 1));
        char *chaineDateProd = strdup(getfield(strdup(ligne), 2));
        char *chaineDateBackUp = strdup(getfield(strdup(ligne), 3));
        dateProd = string_to_date(chaineDateProd);
        dateBackUp = string_to_date(chaineDateBackUp);
        
        //action = csv_analyse_line(dateProd, dateBackUp);
        //action_case_file(action,nomFichier);
        free(nomFichier);
        free(chaineDateProd);
        free(chaineDateBackUp);
    }
    fclose(listCSV);
}

enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp)
{
    time_t origin = time(NULL);
    int diffDate = difftime(dateProd, dateBackUp);
    
    //printf( "Timestamp since January 1, 1970: %ld\n", origin);
    //printf( "Timestamp since January 1, 1970: %ld\n", dateBackUp);
    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&dateProd)); 
    printf("DATE : %s ",buff);


    if (dateBackUp == origin)
        printf("NULL\n");
    else if (diffDate > 0)
        printf("tot\n");
    else if (diffDate == 0)
        printf("same\n");
    else
        printf("ERREUR ?\n");

    return INEXIST;
}

time_t string_to_date(char *chaineDateComplete)
{
    time_t date;
    
    char *token = strtok(chaineDateComplete, " ");
    char* dateChaine = token;
    
    token = strtok(NULL, " ");
    char* heureChaine = token;

    
    struct tm tmdate = {0};

    char *tokenDate = strtok(dateChaine, "/");
    tmdate.tm_mday = atoi(tokenDate);

    tokenDate = strtok(NULL, "/");
    tmdate.tm_mon = atoi(tokenDate);
    //printf("Mois : %s \n",tokenDate);


    tokenDate = strtok(NULL, "/");
    tmdate.tm_year = atoi(tokenDate);
    //printf("Annee : %s \n",tokenDate);

    char *tokenHeure = strtok(heureChaine, ":");
    tmdate.tm_hour = atoi(tokenHeure);
    tokenHeure = strtok(NULL, ":");
    tmdate.tm_min = atoi(tokenHeure);;

    //printf("DATE : %d /  %d / %d  ----  %d : %d\n",tmdate.tm_mday, tmdate.tm_mon, tmdate.tm_year, tmdate.tm_hour, tmdate.tm_min);
   
    date = mktime(&tmdate);

    return date;
}


/***
 * Parametre adresse de mutex
 * 
 * mutex crée dans le main
 * 
 */ 

bool action_case_file(enum caseFile action, char *nomFichierCompare)
{

    /**
     * si action == create
     *      On cree dans backup le fichier create
     * 
     * 
     * si action == update
     *      On update
     *  
     * 
     * 
     */
    bool retour = false;
    char ligne[1024];
    FILE *listCSV = fopen(NAME_LIST, "r");
    FILE *tmp = fopen("temp.csv", "w");
    printf("action en cours...\n");

    if (listCSV == NULL)
    {
        printf("Cannot open file %s\n", NAME_LIST);
    }
    else
    {
        while (fgets(ligne, 1024, listCSV))
        {

            char *nomFichier = strdup(getfield(strdup(ligne), 1));
            char *chaineDateProd = strdup(getfield(strdup(ligne), 2));
            char *chaineDateBackUp = strdup(getfield(strdup(ligne), 3));
            switch (action)
            {
            case CREATE:
                printf("CREATE\n");
                if (strcmp(nomFichier, nomFichierCompare) == 0)
                { // ce if se fera surement en dehors du switch/case
                    // mettre a jour csv:
                    /**
                 * ajouter le nv fichier ac la bonne date à temp dans ce if
                 * et dans le else ajouter la ligne classique
                 */
                    // ajouter le nv fichier dans BackUp
                    break;
                }
                retour = true;
                break;

            case UPDATEB:
                printf("UPDATE\n");
                // mettre a jour csv:
                /**
             * modifier le fichier ac la bonne date à temp dans ce if
             * et dans le else ajouter la ligne classique
             */
                // ajouter le nv fichier dans BackUp:
                /**
             * supprimer le fichier de meme nom dans BackUp
             * ajouter le fichier modifie de prod dans BackUp
             */
                retour = true;
                break;



            case INEXIST:
                printf("ERREUR INEXIST\n");
                break;
            
            case UPDATEP:
                printf("ERREUR DELETE\n");
                break;

            default:
                break;
            }
            free(nomFichier);
            free(chaineDateProd);
            free(chaineDateBackUp);
        }
    }

    fclose(tmp);
    fclose(listCSV);
    remove(NAME_LIST);
    rename("temp.csv", NAME_LIST);

    printf("action bien fini.\n");
    return retour;
}

const char *getfield(char *line, int num)
{
    const char *tok;
    for (tok = strtok(line, ";");
         tok && *tok;
         tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}
