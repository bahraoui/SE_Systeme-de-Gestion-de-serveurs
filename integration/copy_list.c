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
    bool copyData;

    FILE *listCSV = fopen(NAME_LIST, "r");

    if (listCSV == NULL)
    {
        printf("Cannot open file %s\n", NAME_LIST);
        exit(0);
    }
    fgets(ligne, 1024, listCSV);
    while (fgets(ligne, 1024, listCSV))
    {
        char *nomFichier = strdup(get_field(strdup(ligne), 1));
        char *chaineDateProd = strdup(get_field(strdup(ligne), 2));
        char *chaineDateBackUp = strdup(get_field(strdup(ligne), 3));
        dateProd = string_to_date(chaineDateProd);
        dateBackUp = string_to_date(chaineDateBackUp);
        
        action = csv_analyse_line(dateProd, dateBackUp);
        copyData = action_case_file(action,nomFichier);
        generate_logs_stats(nomFichier,action,copyData);

        free(nomFichier);
        free(chaineDateProd);
        free(chaineDateBackUp);
    }
    fclose(listCSV);
}

enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp)
{
    time_t origin = (time_t)0;
    int diffDate = difftime(dateProd, dateBackUp);

    if (dateProd == origin)
        return INEXIST;
    else if (dateBackUp == origin)
        return CREATE;
    else if (diffDate > 0)
        return UPDATEB;
    else if (diffDate < 0)
        return UPDATEP;
    else if (diffDate == 0)
        return UPTODATE;
    
    return UPTODATE;
}

time_t string_to_date(char *chaineDateComplete)
{
    time_t date;

    struct tm tm;
    strptime(chaineDateComplete, "%d/%m/%Y-%H:%M:", &tm);

    date = mktime(&tm);
    date = timegm(localtime(&date));
   

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

            char *nomFichier = strdup(get_field(strdup(ligne), 1));
            char *chaineDateProd = strdup(get_field(strdup(ligne), 2));
            char *chaineDateBackUp = strdup(get_field(strdup(ligne), 3));
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

const char *get_field(char *line, int num)
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


void generate_logs_stats(char* nomFichier, enum caseFile action, bool error) {
    time_t timestamp = time( NULL );
    struct tm * pTime = localtime( & timestamp );

    char buffer[ MAX_SIZE ];
    strftime( buffer, MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime );
    printf( "Date and french time : %s\n", buffer );
}
