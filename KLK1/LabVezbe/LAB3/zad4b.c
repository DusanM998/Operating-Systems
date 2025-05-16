#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_ARGS 20
#define MAX_ARG_LENGTH 100

int main(int argc, char* argv[])
{
    int i;
    char **argumenti = (char**)malloc((argc+1)*sizeof(char*)); //niz nizova karaktera (malloc se koristi da bi se dinamicki kreiralo)
    argumenti[0] = (char*)malloc((strlen("./zad4a")+1) * sizeof(char));
    strcpy(argumenti[0], "./zad4a");
    for(i=0; i < argc; i++)
    {
        argumenti[i] = (char*)malloc((strlen(argv[i])+1) * sizeof(char)); //string u koji kopiramo odg. element
        strcpy(argumenti[i], argv[i]);
    }
    /*Poslednji argument mora da bude NULL da bi ukazivao na kraj vektora sa argumentima*/
    argumenti[argc] = NULL;
    if(fork() == 0)
    { //Izvrsava se novokreirani proces
        if(execv("./zad4a", argumenti) < 0) //Programski kod tekuceg procesa koji je nastao pozivom fork, se kompletno menja izvrsnom datotekom koja je navedena kao 1. argument
        {
            printf("Doslo je do greske!\n");
            exit(1);
        }
    }
    else
        //Izvrsava se proces roditelj koji ceka da se dete zavrsi
        wait(NULL);
    //Oslobadjamo memoriju koja je zauzeta za vektor argumenata
    for(i = 0; i < argc; i++)
        free(argumenti[i]);
    free(argumenti[i]);
    return 0;
}    
