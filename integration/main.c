#include "copy_list.h"
#include "synchro_list.h"

static pthread_mutex_t  mutexProd = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  mutexBackUp = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  mutexLogs = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  mutexStats = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  mutexListe = PTHREAD_MUTEX_INITIALIZER;
static bool serveurProd, serveurBackUp;


int main(int argc, char const *argv[])
{
    serveurProd = test_server("../production/ping","/dev/null"); // remplacer test_server("../production/ping","/dev/null") par test server
    serveurBackUp = test_server("../backup/ping","/dev/null"); // remplacer test_server("../production/ping","/dev/null") par test server
    int compteur = 0;
    while (true)
    {
        sleep(2);
        while(serveurProd){
            printf("Le serveur Production est en ligne.\n");
            sleep(1);
            if (serveurBackUp)
            {
                compteur++;
                printf("modification %d en cours...\n",compteur);
                sleep(2);
                printf("modification %d fini.\n",compteur);

                /*
                printf("modification %d en cours...\n",compteur);
                printf("synchronisation de la liste en cours...\n");
                //synchro_list(&mutexProd,&mutexBackUp,&mutexLogs,&mutexStats,&mutexListe);
                sleep(2);
                printf("synchronisation de la liste fini.\n");
                printf("copie en cours...\n");
                //copy_list(&mutexProd,&mutexBackUp,&mutexLogs,&mutexStats,&mutexListe);
                sleep(2);
                printf("copie fini.\n");
                printf("synchronisation de la liste en cours...\n");
                // synchro_list();
                sleep(2);
                printf("synchronisation de la liste fini.\n");
                printf("modification %d fini.\n",compteur);
                */
            } else {
                printf("Le serveur BackUp n'est plus en ligne.\n");
                printf("Tentative de reconnexion du serveur Backup...\n");
            }
            
            serveurBackUp = test_server("../backup/ping","/dev/null");
            serveurProd = test_server("../production/ping","/dev/null");
        }
        while(serveurBackUp && !serveurProd){
            sleep(1);
            printf("Le serveur production s'est arrete. Le serveur BackUp prend la main\n");

            
            printf("Tentative de reconnexion du serveur Production...\n");
            sleep(2);

            serveurBackUp = test_server("../backup/ping","/dev/null");
            serveurProd = test_server("../production/ping","/dev/null");
        }
        if (!serveurBackUp && !serveurProd)
        {
            printf("Les serveurs BackUp & Production ne sont plus en ligne.\nVeuillez contacter l'administration de PHARMOS et leur indiquer l'erreur suivant : XG98LO\n");
            exit(EXIT_FAILURE);
        }
    }
    
    return 0;
}
