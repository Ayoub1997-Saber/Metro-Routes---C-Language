
/*************************************************** Projet du Langage C *******************************************/

/****************************************************
* 1- Nom du projet : Itin�raires de m�tro             *
* 2- R�alis� par :                                    *
*      Nom : Taihi                                    *
*      Pr�nom : Ayoub                                 *
*****************************************************/

#define _CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <time.h>                  /* destinée à manipuler les trajets entre stations d'une ligne */                                  

#include <malloc.h>

#define TRUE 1

#define FALSE 0

#define LIGNES 14

#define STATIONS 1412 

/// I- définition des structures : ///

        /* 1/ définition de l'horaire */

typedef struct _SCHEDULE {

    int hour;

    int minute;

    int second;

} SCHEDULE;


/* 2/ manipulation des stations */

typedef struct _STATION {

    /* 2/ 1/ pour l'estimation du temps de trajet entre 2 stations */

                 /* - Utilisation d'une liste (organisation des stations selon l'ordre de passage) */


    int CurrentStation;                        // station actuelle //

    int NextStationTime;                       // mesure du temps vers la prochaine station //

    int PrevStationTime;                       // mesure du temps vers la précédente station // 

    int path;                                  // unité pour la mesure du chemin minimal entre stations //                                   

    SCHEDULE firstTrain;                       // le premier train dans une ligne //   

    SCHEDULE lastTrain;                        // le dernier train dans une ligne //

    struct _STATION* head;                     // pointage vers d'autres stations depuis une station source //

    struct _STATION* pnext;                    // liaison avec la prochaine station //

    struct _STATION* pprev;                    // liaison avec la précédente station //

    char* userStation;                        // entrée d'une station que l'utilisateur veut accéder //

    int lineOfStation;                        // situation de la ligne d'une station //                    

} STATION;

/* 3/ manipulation des lignes */

typedef struct _LINE {

    int CurrentLine;                      // ligne actuelle //                                  

    STATION* enterLine;                   // indication pour entrer dans une des lignes //

    STATION* exitLine;                    // indication pour sortir dans une ligne // 

} LINE;

/// II- Définition des fonctions ///


             /* II-1- fonctions d'affichage et de calcul */


void timeOfItinerary(LINE line);                          // calcul du temps de trajet dans une des lignes de métro //
void printStation(STATION* station);                      // affichage d'une des stations de métro //
void printLine(LINE* line);                               // affichage d'une des lignes de métro //  
int countStations(LINE* line);                            // comptage des stations d'une des ligne



           /* II-2- fonctionnalités pour la manipulation des trajets (permettre à l'utilisateur de trouver le meilleur chemin) */



int existStation(LINE line, char* name);                                         // test de l'existence d'une station dans une ligne (indiquer à l'utilisateur la validité de la station qu'il veut la parvenir) //
int existLine(LINE* line);                                                      // test l'existence d'une ligne  
int addStation(STATION* interm, LINE* line);                                   // ajout d'une station dans une ligne (pour éventuellement construire un autre itinéraire) //
STATION* deleteStationFromLine(char* name, STATION* station, LINE* line);      // suppression d'une station existante d'une ligne //
void correspondance(LINE* line);                                              // calcul de la correspondance //


// chargement et manipulation du fichier //


void loadStation(FILE* pstation, LINE* line, char* lineUser);                                    // chargement des stations d'une ligne issues du fichier "metro.csv" §§ 
void loadLine(FILE* pline, LINE line[]);                                                         // chargement des lignes issues du fichier "metro.csv" //                                         
LINE DepartureToArrival(FILE* file);                                                            // chargement de l'itinéraire que doit suivre l'utilisateur //

/// III- Programme principal ///



int main(int argc, char** argv)
{

    /* création du "handle" pour manipuler les données du fichier "metro.csv" */

    FILE* pmetro = fopen("metro.csv", "r");

    if (!pmetro)

    {
        printf("impossible d'ouvrir le fichier!\n");
        exit(1);

    }

    /* ici mettre les fonctionnalités pour manipuler les lignes et les stations de métro issues du fichier "metro.csv" */
    /*puis fermeture du fichier */

    fclose(pmetro);
    return 0;
}





/// VI - définitions des fonctions (on commence d'abord par les affichages du temps ) ///




void timeOfItinerary(LINE line)
{

    SCHEDULE durDirect, durInv;                                       // durDirect = durée du trajet aller -- durInv = durée du trajet retour //

    STATION* dir = line.enterLine;                                   // dir = direction (entrer dans une station d'une des ligne) //  

    /* mesure du trajet aller (calculer au fur et à mesure le trajet entre 2 stations voisines jusqu'au terminal) */

    while (dir != line.exitLine)
    {

        durDirect.hour = (*dir).pnext->lastTrain.hour - dir->lastTrain.hour;
        durDirect.minute = (*dir).pnext->lastTrain.minute - dir->lastTrain.minute;
        durDirect.second = (*dir).pnext->lastTrain.second - dir->lastTrain.second;

        dir = (*dir).pnext;
    }

    dir = line.exitLine;                   // mémoriser le pointeur qui pointe vers la station terminale pour faire l'opération inverse //

    /* mesure du trajet retour (calculer au fur et à mesure le trajet entre 2 stations voisines du terminal jusqu'à l'entrée d'une ligne ) */

    while (dir != line.enterLine)
    {

        durInv.hour = (*dir).lastTrain.hour - dir->pprev->lastTrain.hour;
        durInv.minute = (*dir).lastTrain.minute - dir->pprev->lastTrain.minute;
        durInv.second = (*dir).lastTrain.second - dir->pprev->lastTrain.second;

        dir = (*dir).pprev;
    }


}









void printStation(STATION* station)
{
    printf("Nom de la station : %s\n", (*station).userStation);  // car il s'agit d'une station ou bien du départ ou bien de l'arrivée //
    printf("Trajet parcouru : %d\n", (*station).path);
}

void printLine(LINE* line)
{

    STATION* stationLine;                            // tous les stations d'une seule ligne //

    stationLine = (*line).enterLine;                 // accès à une ligne particulière (passée en argument de printLine) //

    printf("The Parisian metro line :\n");


    if (existLine(line))
    {

        while (stationLine != line->exitLine) // condition d'arrêt qui correspond à la fin d'une ligne (sortie) //
        {

            printStation(stationLine);
            stationLine = (*stationLine).pnext;

        }

        // affichage de la dernière station (qui n'était pas incluse dans la boucle) et sortie d'une ligne //

        printStation((*line).exitLine);

    }

    else

        printf("Sorry, there is an error of entering metro!\n");
}











int countStations(LINE* line)
{

    STATION* stations;
    stations = (*line).enterLine;
    int number_of_stations = 0;

    do {

        number_of_stations = number_of_stations + 1;

        stations = (*stations).pnext;

    } while (stations != NULL);                          // jusqu'à la dernière station d'une des lignes entrées en argument de la fonction 

    return number_of_stations;
}











int existStation(LINE line, char* name)
{

    int exist = TRUE;                                                          // test de l'existence d'une station dans une des lignes passée en argument de la foction // 

    STATION* station = line.enterLine;

    do {
        for (int i = 0; (*station).userStation[i] == name[i] && (*station).userStation[i] == '\0'; i++)
            if (!((*station).userStation[i] > name[i]) && !((*station).userStation[i] < name[i]))                                 // test du nom entré par l'utilisateur (passé en argument de la foction) s'il est valide // 

                return exist;

            else

                return EXIT_FAILURE;

        station = (*station).pnext;

    } while ((*station).pnext != NULL);

    return EXIT_SUCCESS;
}









int existLine(LINE* line)
{

    int exist = TRUE;

    if ((*line).enterLine != NULL || (*line).exitLine != NULL)

        return exist;

    else

        return EXIT_FAILURE;

}








int addStation(STATION* station, LINE* line)
{

    STATION* interm = (*line).enterLine;                            /* "pointeur" intermédiaire (qui sert à réserver de l'emplacement mémoire à une nouvelle station) */
    interm = (*line).enterLine;                                     /* intégrer ce pointeur à l'entrée d'une ligne */

      /* l'ajout se fait au cas par cas */

    do {

        if (interm == (*line).enterLine)                     /* 1- Si l'ajout se fait au début d'une ligne */
        {

            (*interm).pprev = station;                                  /* le pointeur habituel d'une station devient le précédent */

            (*line).enterLine = station;                                /* "redéfinir" l'entrée de la ligne (intègrer le pointeur qui relie la nouvelle station à l'entrée de la ligne) */

            (*station).pnext = interm;                                  /* permuter le pointeur de "station" par le pointeur intermédiaire pour réserver de l'emplacement mémoire */

        }

        else                                                 /* 2- Si l'ajout se fait au milieu ou à la fin de la ligne */
        {

            (*interm).pprev = station;                                /* même explication que 1- */

            (*station).pnext = station;                               /* pour réserver l'emplacement mémoire à la nouvelle station dans la ligne */
                                                                        // mise à jour des 2 pointeurs //
            (*interm).pprev = station;                                /* "redéfinir" le pointeur de la station vers la nouvelle station (qui remplace la 2ème ligne de 1- car on est au milieu/fin d'une ligne) */

            (*station).pnext = interm;                                /* même explication que 1- */
        }

        interm = (*interm).pnext;                            /* 3- Mise à jour du pointeur intermédiaire (pour faire la mise à jour de la nouvelle ligne) */
    } while (interm != NULL);                                /* sortie de la boucle et fin de l'opération */

    interm = (*line).exitLine;                                        /* Mise à jour également du pointeur de la sortie d'une ligne (qui n'était pas inclus dans la boucle) */


}










STATION* deleteStationFromLine(char* name, STATION* station, LINE* line)        // suppression d'une station existante d'une ligne //
{

    station = NULL;                                                             // suppression d'une station dans une ligne //

    if (existLine(line))
    {

        /* 1- le cas où la station se trouve au début d'une ligne */

        if ((*line).enterLine->userStation == name)

        {

            station = (*line).enterLine;                                           // suppression de la station d'entrée d'une ligne // 

            (*line).enterLine = (*line).enterLine->pnext;                          // mise à jour du pointeur de la nouvelle entrée d'une ligne qui se trouve après l'ancienne station d'entrée // 

            (*line).enterLine->pprev = NULL;                                      // la nouvelle station d'entrée ne contient pas le pointeur précédent //

        }


        /* 2- le cas où la station se trouve à la fin d'une ligne */

        else if ((*line).exitLine->userStation == name)

        {

            station = (*line).exitLine;                                         // suppression de la station de sortie d'une ligne //

            (*line).exitLine = (*line).exitLine->pprev;                        // mise à jour du pointeur de la nouvelle sortie d'une ligne qui se trouve avant l'ancienne station de sortie //

            (*line).exitLine->pnext = NULL;                                    // la nouvelle station de sortie ne contient pas le pointeur suivant //

        }

        /* 3- le cas où la station se trouve au milieu d'une ligne */

        else

        {

            station = (*line).enterLine;

            (*line).enterLine = (*line).enterLine->pnext;                      // initialisation du pointeur nul vers la station suivante (et faire le test à partir de la 2ème station d'une ligne) // 

            do {

                if ((*line).enterLine->userStation == name)
                {

                    station = (*station).pnext;                                    // tous les cas où une station peut être au milieu  

                    (*station).pprev = (*station).pnext;                           // pour remplacer la station supprimée par la station qui la précède // 

                    (*station).pnext->pnext = (*station).pnext;                    // mise à jour du pointeur suivant de la station qui remplace la station supprimée //

                    (*station).pnext->pprev = (*station).pprev;                    // mise à jour du pointeur précédent de la station qui remplace la station supprimée //

                }

            } while (station != (*line).exitLine);                                // mise à jour des pointeurs de toutes les stations (le pointeur courant d'une station devient le prochain mis à part le pointeur qui désigne la sortie de la ligne) //



        }
    }

    else
    {

        printf("Sorry, there is an error of entering metro line!\n");

    }
    return station;
}












void correspondance(LINE* line)
{

    STATION* station = (*line).enterLine;

    do {

        if ((*line).enterLine != (*line).enterLine->pnext)
        {

            printLine(line);                                                                 // suivre l'itinéraire qui ne comporte pas la correspondance //
            station = (*station).pnext;                                                        // tester jusqu'à l'arrivée à une destination //

        }


    } while ((*station).pnext != NULL);

}












void loadStation(FILE* pstation, LINE* line, char* lineUser)
{
    //caractères de délimitation//

    char* dieses[17] = { "####1","####2","####3","####3bis","####4","####5","####6","####7","####7bis","####8","####9","####10","####11","####12","####13" };

    char colon[2] = ":";

    // temps de trajet //

    double hourFile;

    /* 1- Allocation dynamique pour extraire les champs de valeurs du fichier ainsi que le nom entré par l'utilisateur */

    STATION* station = (STATION*)calloc(LIGNES, sizeof(STATION));              // allouer dynamiquement les données de chaque station //

    int information;

    do {

        STATION* departure = (STATION*)calloc(1, sizeof(STATION));         // allouer dynamiquement la station de départ pour suivre son chemin // 

        information = 0;                                                   // chaque champ a son propre information

              /* 1-1 Extraire chaque champ du fichier "metro.csv"  */

        char* nameUser = (char*)calloc(LIGNES, sizeof(char));           // allouer dynamiquement le nom de la station entrée par l'utilisateur //   

        fgets(nameUser, LIGNES, pstation);                              // lire le contenu de chaque ligne du fichier "metro.csv" // 

        char* pinfo = strtok(nameUser, colon);                          // identifier toutes les informations séparées par ":" et donner la signification à chacune : 

        while (pinfo != NULL)
        {

            switch (information)

            {

            case 0:                                                 // cas de détection de la ligne //

                (*departure).lineOfStation = atoi(pinfo);            // transformer la chaîne de caractère qui désigne le numéro de la ligne en une valeur numérique //
                break;

            case 1:                     // cas de détermination du sens de trajet (a ou b) //

                while (departure != NULL)
                {
                    if (pinfo == "a")

                        departure = (*departure).pnext;                         // récupérer le sens du trajet aller //

                    else if (pinfo == "b")

                        departure = (*departure).pprev;                         // récupérer le sens du trajet retour //
                }
                break;

            case 2:                     // cas de la détermination de la station //

                (*departure).userStation = (char*)calloc(strlen(pinfo) + 1, sizeof(char));              // allouer dynamiquement le nom entré par l'utilisateur //

            case 3:
                break;
            case 4:
                break;

            case 5:                     // cas de la détermination de l'heure de départ du 1er métro //

                hourFile = atof(pinfo);                                                        // convertir l'heure de départ en une virgule flottante // 

                (*departure).firstTrain.hour = (int)hourFile;                                  // stocker chaque heure pour les manipuler dans la suite (stocker des entiers de la virgule flottante car les mantisses correspondent aux minutes) //

                break;

            case 6:                    // cas de la détermination de l'heure de passage du dernier métro //

                hourFile = atof(pinfo);

                (*departure).lastTrain.hour = (int)hourFile;

                break;

            default:

                printf("error of entering metro!\n");


            }

            information++;                                              // passer par tous les cas // 

            pinfo = strtok(NULL, colon);                                   // après le passage par chaque cas, on initialise "pinfo" par 0 (pour ne pas garder les informations précédentes) // 
        }
    } while (information <= 6);

    /* 2- Détection de la ligne où se trouve la station */

    int i = 0;
    while (dieses[i] != '\0')
        if ((&lineUser[i]) == dieses[i])
        {
            (*line).CurrentLine = (i + 1);
            i++;
        }

    /***************************
for (int i = 1; i < LIGNES; i++)

if (lineUser == "####1" && i == 1)                                          (*line).CurrentLine = i;

else if (lineUser == "####2" && i == 2)                                     (*line).CurrentLine = i;

else if ((lineUser == "####3" || lineUser == "####3bis") && i == 3 )        (*line).CurrentLine = i;

else if (lineUser == "####4" && i == 4)                                     (*line).CurrentLine = i;

else if (lineUser == "####5" && i == 5)                                     (*line).CurrentLine = i;

else if (lineUser == "####6" && i == 6)                                     (*line).CurrentLine = i;

else if ((lineUser == "####7" || lineUser == "####7bis") && i == 7)         (*line).CurrentLine = i;

else if (lineUser == "####8" && i == 8)                                     (*line).CurrentLine = i;

else if (lineUser == "####9" && i == 9)                                     (*line).CurrentLine = i;

else if (lineUser == "####10" && i == 10)                                   (*line).CurrentLine = i;

else if (lineUser == "####11" && i == 11)                                   (*line).CurrentLine = i;

else if (lineUser == "####12" && i == 12)                                   (*line).CurrentLine = i;

else if (lineUser == "####13" && i == 13)                                   (*line).CurrentLine = i;

else                                                                        (*line).CurrentLine = i;


*************************/
}





































void loadLine(FILE* pline, LINE line[])
{

    char* dieses[16] = { "####1","####2","####3","####3bis","####4","####5","####6","####7","####7bis","####8","####9","####10","####11","####12","####13" };

    int i;

    for (i = 1; i <= 16; i++)

        while (dieses[i] != '\0')
        {

            loadStation(pline, &line[i], dieses[i]);                                              /* charger toutes les stations la ligne */

        }

}




















LINE DepartureToArrival(FILE* file)
{

    LINE line = { 0,NULL,NULL };

    loadLine(file, &line);

    char* diesesD[16] = { "####1","####2","####3","####3bis","####4","####5","####6","####7","####7bis","####8","####9","####10","####11","####12","####13" };

    int i;
    for (i = 1; i <= 16; i++)

        while (diesesD[i] != '\0')
        {

            loadStation(file, &line, diesesD[i]);                                              /* charger toutes les stations la ligne de départ */

        }


    char* diesesA[16] = { "####1","####2","####3","####3bis","####4","####5","####6","####7","####7bis","####8","####9","####10","####11","####12","####13" };

    for (i = 1; i <= 16; i++)

        while (diesesA[i] != '\0')
        {

            loadStation(file, &line, diesesA[i]);                                              /* charger toutes les stations la ligne d'arrivée */

        }

    STATION* departure = line.enterLine;
    STATION* arrival = line.exitLine;

    // construire un itinéraire (pas forcément le plus court) //

    while (departure != NULL)
    {
        do {

            if ((*departure).userStation == (*arrival).userStation)
                deleteStationFromLine((*arrival).userStation, arrival, &line);                                   /* si au pire l'utilisateur rentre une station la même que celle de l'entrée on la supprime */

            arrival = (*arrival).pprev;  /* mise à jour du pointeur de l'arrivée précédent après suppression de sa station */

        } while (arrival != line.enterLine);                                            // parcourir tous le chemin jusqu'à l'arrivée à l'entrée de la ligne //

        arrival = line.exitLine;                                                           // mise à jour du nouveau pointeur de l'arrivée 

        departure = (*departure).pnext;                                                    // mise à jour jusqu'à l'arrivée de tous les pointeurs qui précèdent la station de départ 
    }

    return line;                                                                           // avoir toute la "carte" (itinéraire) du départ jusqu'à l'arrivée //
}