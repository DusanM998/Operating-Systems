/*Linux program koji sinhronizuje sadrzaj dva direktorijuma. Putanje do direktorijuma koji se sinhronizuju se zadaju kao ulazni argumenti programa. Program za svaku regularnu datoteku iz prvog direktorijuma proverava da li postoji odgovarajuca datoteka u drugom direktorijumu (uporedjuje ime i velicinu datoteke). Ukoliko datoteka ne postoji onda se ona kopira iz prvog u drugi direktorijum. (Pretpostaviti da prvi direktorijum ne sadrzi poddirektorijume)*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char* argv[])
{
    struct stat status;
    DIR* direktorijum;
    struct dirent* stavka;
    char stavkaPath1[300];
    char stavkaPath2[300];
    
    DIR* direktorijum2;
    struct dirent* stavka2;
    struct stat status2;
    
    int pid;
    int retVal;
    
    if(argc < 3)
    {
        printf("Pogresni ulazni argumenti!\n");
        exit(-1);
    }
    retVal = stat(argv[1], &status);
    if(retVal == -1)
    {
        printf("Greska pri prvom statu!\n");
        exit(-1);
    }
    
    if(S_ISDIR(status.st_mode))
    {
        direktorijum = opendir(argv[1]);
        while((stavka=readdir(direktorijum))!= NULL)
        {
            if(strcmp(stavka->d_name, ".") != 0 && strcmp(stavka->d_name, ".") != 0)
            {
                strcpy(stavkaPath2, argv[1]);
                strcat(stavkaPath1, stavka->d_name);
                
                stat(stavkaPath1, &status);
                
                direktorijum2 = opendir(argv[2]);
                int pom = 0;
                while((stavka2 = readdir(direktorijum2)) != NULL)
                {
                    strcpy(stavkaPath2, argv[2]);
                    strcat(stavkaPath2, stavka2->d_name);
                    
                    stat(stavkaPath2, &status);
                    if(strcmp(stavka2->d_name, stavka->d_name) == 0 && status2.st_size == status.st_size)
                    {
                        pom = 1;
                        break;
                    }
                }
                if(pom == 0)
                {
                    strcpy(stavkaPath2, argv[2]);
                    strcat(stavkaPath2, stavka->d_name);
                    pid = fork();
                    if(pid == 0)
                    {
                        execlp("cp", "-r", "cp", stavkaPath1, stavkaPath2, NULL);
                        printf("Neuspesno kopiranje!\n");
                        exit(-1);
                    }
                    wait(NULL);
                }
            }
        }
    }
    else
        printf("Prvi ulazni argument ne predstavlja direktorijum!\n");
    exit(-1);
}
                
