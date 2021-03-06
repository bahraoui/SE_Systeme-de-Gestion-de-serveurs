#include "copy_list.h"

/**
 * Le nombre de fichier recu par le module
 */
int recu = 0;
/**
 * Le nombre de mofication reussi
 */
int success = 0;
/**
 * Le nombre de mofication echoué
 */
int failed = 0;

//Fonction principale du module COPY LIST
void copy_list(pthread_mutex_t*  mutexProd,pthread_mutex_t*  mutexBackUp,pthread_mutex_t*  mutexLogs,pthread_mutex_t*  mutexStats, pthread_mutex_t* mutexListe)
{
    //Déclaration des variables
    char ligne[1024];
    enum caseFile action;
    time_t dateProd;
    time_t dateBackUp;
    bool copyData;
    // On lock les mutex corresopndants pour avoir l'acces
    pthread_mutex_lock(mutexListe);
    pthread_mutex_lock(mutexProd);
    pthread_mutex_lock(mutexBackUp);
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
        copyData = action_case_file(action,nomFichier);
        
        //On ajoute une ligne dans le fichier de logs
        pthread_mutex_lock(mutexLogs);
        generate_logs(nomFichier,action,copyData);
        pthread_mutex_unlock(mutexLogs);

        //On incrémente le nombre de fichier recu
        recu++;

        //On libère les accès mémoire
        free(nomFichier);
        free(chaineDateProd);
        free(chaineDateBackUp); 
    }
    pthread_mutex_unlock(mutexProd);
    pthread_mutex_unlock(mutexBackUp);
    //stats
    pthread_mutex_lock(mutexStats);
    generate_stats(); //On ajoute une ligne dans le fichier de stats avec les fichiers recu, le nombre de succes et d'echecs
    pthread_mutex_unlock(mutexStats);
    // fermeture du fichier
    printf("Nombre de fichier recu :%d \n",recu);
    printf("Succes :%d \n",success);
    printf("Fail :%d \n",failed);
    fclose(listCSV);
    pthread_mutex_unlock(mutexListe);
}

//On a analyse deux dates pour savoir quelle action a effectué sur le fichier
enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp)
{
    //Date origin = NULL
    time_t origin = (time_t)0;
    int diffDate = difftime(dateProd, dateBackUp);    

    /*printf("DATE PROD : %ld\n",dateProd);
    printf("DATE BACKUP: %ld\n",dateBackUp);
    printf("DATE Origin: %ld\n",origin);*/

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


bool action_case_file(enum caseFile action, char *nomFichierCompare)
{
    bool codeDeRetour = false;
    printf("ACTION : %d\n",action);
    switch (action)
        {
        case CREATE: ;
            // ajouter le nv fichier dans BackUp
            char chemin[MAX_PATH_SIZE] = "";
            strcat(chemin,NAME_PROD);
            strcat(chemin,nomFichierCompare);
            transfert(chemin,NAME_BACKUP);
            codeDeRetour = true;
            break;

        case UPDATEB: ;
            // ajouter le nv fichier dans BackUp:
            /**
             * supprimer le fichier de meme nom dans BackUp
             * ajouter le fichier modifie de prod dans BackUp
             */
            printf("DEDANS\n");
            char chemin2[MAX_PATH_SIZE] = "";
            strcat(chemin2,NAME_BACKUP);
            strcat(chemin2,nomFichierCompare);
            remove(chemin2);
            char cheminb2[MAX_PATH_SIZE] = "";
            strcat(cheminb2,NAME_PROD);
            strcat(cheminb2,nomFichierCompare);
            transfert(cheminb2,NAME_BACKUP);
            printf("APRES\n");
            codeDeRetour = true;
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
            remove(chemin3); // prod/a
            char cheminb3[MAX_PATH_SIZE] = "";
            strcat(cheminb3,NAME_BACKUP);
            strcat(cheminb3,nomFichierCompare);
            transfert(cheminb3,NAME_PROD);
            codeDeRetour = true;
            break;

        case INEXIST: ;
            // ajouter le nv fichier dans Prod
            char chemin4[MAX_PATH_SIZE] = "";
            strcat(chemin4,NAME_BACKUP);
            strcat(chemin4,nomFichierCompare);
            transfert(chemin4,NAME_PROD);
            codeDeRetour = true;
            break;

        

        default:
            break;
        }

    printf("action bien fini.\n");
    return codeDeRetour;
}

//Génère la ligne de statistique et l'ajoute dans le fichier stats.csv
void generate_stats(){
    time_t timestamp = time( NULL );
    FILE *fileStats;

    //On récupère la date du jour
    char curentDate[MAX_SIZE];
    struct tm * pTime = localtime(&timestamp);
    char messageCSV[MAX_SIZE] = "";
    strftime(curentDate, MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime );
    strcat(messageCSV,curentDate);

    strcat(messageCSV,";");

    //On indique le nombre de succès
    char recuChaine[12];
    sprintf(recuChaine, "%d", recu);
    strcat(messageCSV,recuChaine);
    strcat(messageCSV,";");

    //On indique le nombre de succès
    char sucessChaine[12];
    sprintf(sucessChaine, "%d", success);
    strcat(messageCSV,sucessChaine);
    strcat(messageCSV,";");


    //On indique le nombre d'echecs
    char failedChaine[12];
    sprintf(failedChaine, "%d", failed);
    strcat(messageCSV,failedChaine);

    strcat(messageCSV,"\n");

    //On ajoute la ligne dans le fichier
    fileStats = fopen(NAME_STAT_COPY, "a");
    fprintf(fileStats,"%s",messageCSV);
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
    fprintf(fileLog,"%s",messageLogs);
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
    strcat(commandeFinal," "); // on enleve la sortie de la commande pour un affichage plus propre
    /**
     * On execute la commande recuperer et on retourne la valeur de retour de la commande pour savoir 
     * si l'execution s'est bien passe
     */
    if(!system(commandeFinal)){
        printf("Transfert de %s réussi.\n",ficSrc);
        success++;
        return true;
    } else {
        printf("Le systeme a essaye en vain d'executer cette commande : %s\n",commandeFinal);
        printf("Le transfert ne s'est pas bien passe car  soit:\n\t1. vous n'avez pas installe ssh : solution : \"sudo apt-get install ssh\"\n\t2. vous n'avez pas lance ssh : solution \"sudo service ssh start\"\n\t3. Pour tout autre problème, veuillez verifier votre systeme UNIX ou contacter notre responsable technique : \"alexandre.khel@gmail.com\" .\n\n");
        failed++;
        return false;
    }
    return false;
}

bool test_server(char* ficSrc,char* destination){
    if (realpath(ficSrc,NULL)==NULL) // on verifie bien que le fichier existe
        return false;
    char commandeFinal[MAX_PATH_SIZE] = "scp "; // la commande final a executer
    char utilisateur[MAX_UTILISATEUR]; // l'utilisateur actuel
    cuserid(utilisateur); // attribution de l'utilisateur actuel
    strncat(commandeFinal,utilisateur,taille_char(utilisateur)); // on ajoute l'utilistaeur
    strcat(commandeFinal,"@localhost:"); // on ajoute l'adresse de la machine distante
    strcat(commandeFinal,realpath(ficSrc,NULL)); // on ajoute le fichier a copier
    strcat(commandeFinal," "); 
    strcat(commandeFinal,destination); // on ajoute la destination ou l'on copiera le fichier
    strcat(commandeFinal," 1>/dev/null 2>/dev/null "); // on enleve la sortie de la commande pour un affichage plus propre
    /**
     * On execute la commande recuperer et on retourne la valeur de retour de la commande pour savoir 
     * si l'execution s'est bien passe
     */
    if(!system(commandeFinal)){
        printf("Connexion au serveur réussi.\n");
        success++;
        return true;
    } else {
        printf("Le systeme a essaye en vain d'executer cette commande : %s\n",commandeFinal);
        printf("Le transfert ne s'est pas bien passe car  soit:\n\t1. vous n'avez pas installe ssh : solution : \"sudo apt-get install ssh\"\n\t2. vous n'avez pas lance ssh : solution \"sudo service ssh start\"\n\t3. Pour tout autre problème, veuillez verifier votre systeme UNIX ou contacter notre responsable technique : \"alexandre.khel@gmail.com\" .\n\n");        failed++;
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

    tm.tm_isdst = -1;
    date = mktime(&tm);
    date = timegm(localtime(&date));
   

    return date;
}

//On récupère un élément d'une ligne du csv selon un indice
const char *get_field(char *ligne, int num)
{
    const char *delimiteur;
    for (delimiteur = strtok(ligne, ";");
         delimiteur && *delimiteur;
         delimiteur = strtok(NULL, ";\n"))
    {
        if (!--num)
            return delimiteur;
    }
    return NULL;
}

//Donne le nombre de caractère d'une chaine donné en paramètre
int taille_char(char *str){
    int i=0;
    for (i = 0; i < sizeof(str); i++)
        if (str[i]=='\0')
            return i;
    return 0;    
}
