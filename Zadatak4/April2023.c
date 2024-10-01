/*Linux program koji u zadatom direktorijumu(kao ulazni argument programa zadaje se apsolutna putanja do direktorijuma)
 * i njegovim poddirektorijumima (maximalno do nivoa 5) odredjuje i stampa naziv najvece regularne datoteke.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_NAME 1024

// Globalne promenljive za čuvanje informacije o najvećoj datoteci
char maxpath[MAX_NAME] = "";
off_t maxsize = 0;

void processdir(char *path, int level)
{
    printf("Procesuiram direktorijum: %s: | nivo: %d\n", path, level);
    
    if(level > 5)
    {
        printf("Dostigao sam maksimalnu dubinu. Izlazim...\n");
        return;
    }
    
    char path1[MAX_NAME];
    struct stat statbuf;
    DIR *dp;
    struct dirent *dirp;
    
    dp = opendir(path);
    
    if(dp == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma!\n");
        exit(-1);
    }
    
    while((dirp = readdir(dp)) != NULL)
    {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        
        strcpy(path1, path);
        strcat(path1, "/");
        strcat(path1, dirp->d_name);
        
        stat(path1, &statbuf);
        
        if(S_ISREG(statbuf.st_mode))
        {
            if(statbuf.st_size > maxsize)
            {
                maxsize = statbuf.st_size;
                strncpy(maxpath, path1, sizeof(maxpath) - 1);
                maxpath[sizeof(maxpath) - 1] = '\0';  // Sigurnosni terminator
            }
        }
        else if(S_ISDIR(statbuf.st_mode) && level < 5)
        {
            processdir(path1, level + 1);
        }
    }
    
    //printf("Datoteka %s ima maksimalnu velicinu: %dB\n", maxpath, size);
    
    closedir(dp);
}

int main(int argc, char *argv[])
{
    //char dirname[MAX_NAME];
    struct stat statbuf;
    
    if(argc < 2)
    {
        printf("Nema dovoljno ulaznih argumenata!\nPokrenite program: %s <putanja_do_direktorijuma>\n", argv[0]);
        exit(-1);
    }
    
    //strcpy(dirname, argv[1]);
    
    if(stat(argv[1], &statbuf) < 0)
    {
        printf("Doslo je do greske prilikom ocitavanja statusa datoteke!\n");
        exit(-1);
    }
    
    if(!S_ISDIR(statbuf.st_mode))
    {
        printf("Uneti argument ne predstavlja putanju do direktorijuma!\n");
        exit(-1);
    }
    
    processdir(argv[1], 1);
    
    if (maxsize > 0) 
    {
        printf("Najveca datoteka: %s sa veličinom: %ld B\n", maxpath, (long int)maxsize);
    } 
    else 
    {
        printf("Nema regularnih datoteka u direktorijumu ili njegovim poddirektorijumima.\n");
    }
    
    return 0;
    
}
