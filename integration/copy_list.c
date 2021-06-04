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

    
    //parcours chaque ligne du CSV
    char ligne[1024];
    enum caseFile action;
    time_t dateProd;
    time_t dateBackUp;

    FILE * listCSV = fopen(NAME_LIST, "r" );

    if ( listCSV == NULL ) {
        printf( "Cannot open file %s\n", NAME_LIST );
        exit( 0 );
    }
    while (fgets(ligne, 1024, listCSV))
	{
               
        char* nomFichier = strdup(getfield(strdup(ligne),1));
        char *chaineDateProd = strdup(getfield(strdup(ligne),2));
        char* chaineDateBackUp = strdup(getfield(strdup(ligne),3));

        dateProd = string_to_date(chaineDateProd);
        dateBackUp = string_to_date(chaineDateBackUp);

        split_data(ligne,nomFichier,&dateProd,&dateBackUp); 
        action = csv_analyse_line(dateProd, dateBackUp);
        action_case_file(action,nomFichier);     


        free(nomFichier);
        free(chaineDateProd);
        free(chaineDateBackUp);

	}
    fclose(listCSV);

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



const char* getfield(char* line, int num)
{
	const char* tok;
	for (tok = strtok(line, ";");
			tok && *tok;
			tok = strtok(NULL, ";\n"))
	{
		if (!--num)
			return tok;
	}
	return NULL;
}
 