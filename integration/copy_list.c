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
        //copyData = action_case_file(action,nomFichier);
        generate_logs_stats(nomFichier,action,true);

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

bool action_case_file(enum caseFile action, char *nomFichierCompare,int numLigne)
{
    bool codeDeRetour = false;
    printf("action en cours...\n");
    switch (action)
        {
        case CREATE: ;
            // ajouter le nv fichier dans BackUp
            char chemin[MAX_PATH_SIZE] = "";
            strcat(chemin,NAME_BACKUP);
            strcat(chemin,nomFichierCompare);
            remove(chemin);
            transfert(chemin,NAME_BACKUP);
            codeDeRetour = true;
            break;

        case UPDATEB: ;
            // ajouter le nv fichier dans BackUp:
            /**
             * supprimer le fichier de meme nom dans BackUp
             * ajouter le fichier modifie de prod dans BackUp
             */
            char chemin2[MAX_PATH_SIZE] = "";
            strcat(chemin2,NAME_BACKUP);
            strcat(chemin2,nomFichierCompare);
            remove(chemin2);
            transfert(chemin2,NAME_BACKUP);
            codeDeRetour = true;
            break;

        case INEXIST:
            printf("ERREUR INEXIST\n");
            break;

        case UPDATEP: ;
            // ajouter le nv fichier dans Prod:
            /**
             * supprimer le fichier de meme nom dans BackUp
             * ajouter le fichier modifie de prod dans BackUp
             */
            char chemin3[MAX_PATH_SIZE] = "";
            strcat(chemin3,NAME_PROD);
            strcat(chemin3,nomFichierCompare);
            remove(chemin3);
            transfert(chemin3,NAME_PROD);
            codeDeRetour = true;
            break;

        default:
            break;
        }

    printf("action bien fini.\n");
    return codeDeRetour;
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
    FILE *fileLog;
    char curentDate[ MAX_SIZE ];
    struct tm * pTime = localtime(&timestamp);
    char messageLogs[MAX_MESSAGE] = "";
    char messageCSV[MAX_SIZE] = "";
    
    //Date du jour
    strftime(curentDate, MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime );

    strcat(messageLogs,curentDate);

    switch (action)
    {
    case CREATE:
        if (error){
            strcat(messageLogs," : [CREATE-SUCCESS] ");
            strcat(messageLogs,nomFichier);
            strcat(messageLogs," in BACKUP\n"); 
        }
            
        else {
            strcat(messageLogs," : [CREATE-FAILED] ");  
            strcat(messageLogs,nomFichier);
            strcat(messageLogs," in BACKUP\n"); 
        }
        break;
    case UPDATEB:
        if (error) {
            strcat(messageLogs," : [UPDATE-SUCCESS] ");
            strcat(messageLogs, nomFichier);
            strcat(messageLogs," from PROD to BACKUP \n"); 
        }
            
        else {
            strcat(messageLogs," : [UPDATE-FAILED] ");
            strcat(messageLogs, nomFichier);
            strcat(messageLogs," from PROD to BACKUP \n"); 
        }
        break;
    case UPDATEP:
        if (error) {
            strcat(messageLogs," : [UPDATE-SUCCESS] ");
            strcat(messageLogs,nomFichier);
            strcat(messageLogs," from BACKUP to PROD \n");
        }   
        else {
            strcat(messageLogs," : [UPDATE-FAILED] ");
            strcat(messageLogs,nomFichier);
            strcat(messageLogs," from BACKUP to PROD \n");
        }
        break;
    case INEXIST:
        if (error) {
            strcat(messageLogs," : [CREATE-SUCCESS] ");
            strcat(messageLogs,nomFichier);
            strcat(messageLogs," nomFichier in PROD \n");
        }
        else {
            strcat(messageLogs," : [CREATE-FAILED] ");
            strcat(messageLogs,nomFichier);
            strcat(messageLogs," nomFichier in PROD \n");

        } 
        break;
    default:
        break;
    }
    fileLog = fopen("logs.txt", "a");
    fprintf(fileLog,messageLogs);
    fclose(fileLog);
}


bool transfert(char* ficSrc,char* destination){
    char commandeFinal[MAX_PATH_SIZE] = "scp -p "; // la commande final a executer
    char utilisateur[MAX_UTILISATEUR]; // l'utilisateur actuel
    cuserid(utilisateur); // attribution de l'utilisateur actuel
    strncat(commandeFinal,utilisateur,taille_char(utilisateur)); // on ajoute l'utilistaeur
    strcat(commandeFinal,"@localhost:"); // on ajoute l'adresse de la machine distante
    strcat(commandeFinal,realpath(ficSrc,NULL)); // on ajoute le fichier a copier
    strcat(commandeFinal," "); 
    strcat(commandeFinal,destination); // on ajoute la destination ou l'on copiera le fichier
    strcat(commandeFinal," 1>/dev/null "); // on enleve la sortie de la commande pour un affichage plus propre
    /**
     * On execute la commande recuperer et on retourne la valeur de retour de la commande pour savoir 
     * si l'execution s'est bien passe
     */
    if(!system(commandeFinal)){
        printf("bonne fin\n");
        return true;
    } else {
        printf("Le transfert n'a pas pu bien se passer, a cause de soit:\n");
        return false;
    }
    return false;
}


int taille_char(char *str){
    int i=0;
    for (i = 0; i < sizeof(str); i++)
        if (str[i]=='\0')
            return i;
    return 0;    
}
