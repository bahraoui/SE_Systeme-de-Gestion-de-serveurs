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
    serveurProd = test_server("../production/ping","/dev/null"); // remplacer transfert("../production/ping","/dev/null") par test server
    serveurBackUp = test_server("../backup/ping","/dev/null"); // remplacer transfert("../production/ping","/dev/null") par test server
    int compteur = 0;
    while (true)
    {
        while(serveurProd){
            compteur++;
            sleep(3);
            if (serveurBackUp)
            {
                printf("modification en cours...\n");
                printf("synchronisation de la liste en cours...\n");
                // synchro_list();
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
                printf("modification en cours...\n");
            }
            
            
            serveurProd = test_server("../production/ping","/dev/null");
        }
        while(serveurBackUp && !serveurProd){
            compteur++;
            serveurBackUp = test_server("../backup/ping","/dev/null");
        }
        if (!serveurBackUp && !serveurProd)
        {
            printf("Les serveurs BackUp & Production ne sont plus en ligne.\nVeuillez contacter l'administration de PHARMOS et leur indiquer l'erreur suivant : XG98LO\n");
            exit(EXIT_FAILURE);
        }
        
    }
    
    return 0;
}
