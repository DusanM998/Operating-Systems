#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pid;
    char datoteka[100];
    /*Sa standardnog ulaza citamo ime datoteke koju zelimo da kopiramo*/
    scanf("%s", datoteka);
    pid = fork();
    
    if(pid == 0)
    {
        if(execlp("cp", "cp", datoteka, "./tmp", NULL) < 0) //Izvrsava cp komandu sa imenom cp, ciji je 1. argument izvorisna datoteka, 2. argument je odredisni folder i NULL predstavlja kraj liste argumenata
        {
            printf("Doslo je do greske!\n");
            exit(1);
        }
    }
    else
        /*Izvrsava se proces roditelj koji ceka da se njegov proces dete zavrsi odnosno da se kopiranje izvrsi*/
        wait(NULL);
    return 0;
}
