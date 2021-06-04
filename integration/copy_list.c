#include "copy_list.h"


void copy_list() {
    

    //A FAIRE DANS LE MAIN

    /*if (false){
        printf("PING PAS PASSE");
        exit(EXIT_FAILURE);
    }

    if (!lock_file()) { //lock list_fic.csv
        printf("PAS REUSSI A VERROUILLER");
        exit(EXIT_FAILURE);
    }*/

    //recup taille list_fic.csv
    int ligneListCSV = 1;
    //On recupere le contenu du fichier list_fic.csv
    
    //parcours chaque ligne du CSV
    int i = 0;
    char* ligne = NULL;
    char* nomFichier = NULL;
    time_t dateProd;
    time_t dateBackUp;

    enum caseFile action;

    for (i = 0; i <ligneListCSV; i++) {
        //recup la ligne
        split_data(ligne,nomFichier,&dateProd,&dateBackUp); 
        action = csv_analyse_line(dateProd, dateBackUp);
        action_case_file(action,nomFichier); 
    }
}


enum caseFile csv_analyse_line(time_t dateProd, time_t dateBackUp) {
    /**
     * difference entre les deux dates
     *  Si la date de modification du serv Back up est NULL
            return CREATE

        SINON Si la date du serv Prod > la date du serv Back up
            return UPDATE        
     * 
     */ 
    return CREATE;
    
}

void split_data(char* ligne,char* nomFichier,time_t* dateProd,time_t* dateBackup) {
    /**
     * On split la ligne
     * On transforme les chaines des dates en type date
     * On update les variables
     */ 
}

time_t string_to_date(char* chaineDate) {
    time_t begin = time( NULL );
    return begin;
}

bool action_case_file(enum caseFile action, char* nomFichier) {

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

    return false;
}



    