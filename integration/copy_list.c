#include "copy_list.h"

//Fonction principale du module COPY LIST
void copy_list()
{
    //Déclaration des variables
    char ligne[1024];
    enum caseFile action;
    time_t dateProd;
    time_t dateBackUp;
    bool copyData;

    //On ouvre la liste de fichier 
    FILE *listCSV = fopen(NAME_LIST, "r");
    if (listCSV == NULL)
    {
        printf("Cannot open file %s\n", NAME_LIST);
        exit(0);
    }
    fgets(ligne, 1024, listCSV);

    //On parcourt chaque ligne du CSV
    while (fgets(ligne, 1024, listCSV))
    {
        //On récupère pour chaque ligne, le nom du fichier, la date de modification dans le serveur Production et dans le serveur BackUp
        char *nomFichier = strdup(get_field(strdup(ligne), 1));
        char *chaineDateProd = strdup(get_field(strdup(ligne), 2));
        char *chaineDateBackUp = strdup(get_field(strdup(ligne), 3));
        dateProd = string_to_date(chaineDateProd);
        dateBackUp = string_to_date(chaineDateBackUp);
        
        //On analyse l'action a effectué selon les dates
        action = csv_analyse_line(dateProd, dateBackUp);

        //On effectue l'action pour le fichier récupérer
        //copyData = action_case_file(action,nomFichier);
        
        //On ajoute une ligne dans le fichier de logs
        generate_logs(nomFichier,action,true);

        //On incrémente le nombre de fichier recu
        recu++;

        //On libère les accès mémoire
        free(nomFichier);
        free(chaineDateProd);
        free(chaineDateBackUp); 
    }
    //On ajoute une ligne dans le fichier de stats avec les fichiers recu, le nombre de succes et d'echecs
    generate_stats();
    fclose(listCSV);
}

//On a analyse deux dates pour savoir quelle action a effectué sur le fichier
enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp)
{
    //Date origin = NULL
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


//On récupère le fichier et l'action a faire et on l'execute
bool action_case_file(enum caseFile action, char *nomFichierCompare)
{
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

            //if (strcmp(nomFichier, nomFichierCompare) == 0) {
            char *nomFichier = strdup(get_field(strdup(ligne), 1));
            char *chaineDateProd = strdup(get_field(strdup(ligne), 2));
            char *chaineDateBackUp = strdup(get_field(strdup(ligne), 3));
            if (strcmp(nomFichier, nomFichierCompare) == 0)
            {
                switch (action)
                {
                case CREATE:
                    printf("CREATE\n");
                    // mettre a jour csv:
                    /**
                     * ajouter le nv fichier ac la bonne date à temp dans ce if
                     * et dans le else ajouter la ligne classique
                     */
                    // ajouter le nv fichier dans BackUp
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
                    remove(nomFichierCompare);
                    transfert(nomFichierCompare,"backup");
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

//Génère la ligne de statistique et l'ajoute dans le fichier stats.csv
void generate_stats(){
    time_t timestamp = time( NULL );
    FILE *fileStats;

    //On récupère la date du jour
    char curentDate[MAX_SIZE];
    struct tm * pTime = localtime(&timestamp);
    char messageCSV[MAX_SIZE] = "\n";
    strftime(curentDate, MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime );
    strcat(messageCSV,curentDate);

    //On indique que l'on est dans le module COPY
    strcat(messageCSV,";COPY;");

    //On indique le nombre de succès
    char sucessChaine[12];
    sprintf(sucessChaine, "%d", success);
    strcat(messageCSV,sucessChaine);
    strcat(messageCSV,";");


    //On indique le nombre d'echecs
    char failedChaine[12];
    sprintf(failedChaine, "%d", failed);
    strcat(messageCSV,failedChaine);

    //On ajoute la ligne dans le fichier
    fileStats = fopen("stats.csv", "a");
    fprintf(fileStats,messageCSV);
    fclose(fileStats);
}

//Génère la ligne de logs et l'ajoute dans le fichier logs.txt
void generate_logs(char* nomFichier, enum caseFile action, bool error) {
    time_t timestamp = time( NULL );
    FILE *fileLog;
    
    //On récupère la date du jour
    char curentDate[MAX_SIZE];
    struct tm * pTime = localtime(&timestamp);
    char messageLogs[MAX_MESSAGE] = "";
    strftime(curentDate, MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime);
    strcat(messageLogs,curentDate);
    
    //Selon l'action qui a été effectué on crée la ligne de logs
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

    //On ajoute la ligne de logs dans le fichier
    fileLog = fopen("logs.txt", "a");
    fprintf(fileLog,messageLogs);
    fclose(fileLog);
}


bool transfert(char* ficSrc,char* destination){
    char commandeFinal[MAX_PATH_SIZE] = "scp "; // la commande final a executer
    char utilisateur[MAX_UTILISATEUR]; // l'utilisateur actuel
    cuserid(utilisateur); // attribution de l'utilisateur actuel
    strncat(commandeFinal,utilisateur,taille_char(utilisateur)); // on ajoute l'utilistaeur
    strcat(commandeFinal,"@localhost:"); // on ajoute l'adresse de la machine distante
    strcat(commandeFinal,realpath("stats.csv",NULL)); // on ajoute le fichier a copier
    strcat(commandeFinal," "); 
    strcat(commandeFinal,destination); // on ajoute la destination ou l'on copiera le fichier
    strcat(commandeFinal," 1>/dev/null "); // on enleve la sortie de la commande pour un affichage plus propre
    /**
     * On execute la commande recuperer et on retourne la valeur de retour de la commande pour savoir si l'execution s'est bien passe
     */
    if(!system(commandeFinal)){
        printf("bonne fin\n");
        success++;
        return true;
    } else {
        printf("Le transfert n'a pas pu bien se passer, a cause de soit:\n");
        failed++;
        return false;
    }
    return false;
}

//On transforme une chaine de caractere en une date de type time_t
time_t string_to_date(char *chaineDateComplete)
{
    time_t date;

    struct tm tm;
    strptime(chaineDateComplete, "%d/%m/%Y-%H:%M:", &tm);

    date = mktime(&tm);
    date = timegm(localtime(&date));
   

    return date;
}

//On récupère un élément d'une ligne du csv selon un indice
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

//Donne le nombre de caractère d'une chaine donné en paramètre
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
