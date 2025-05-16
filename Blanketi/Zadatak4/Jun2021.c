/*Linux program koji u zadatom direktorijumu rekurzivno(maksimalno do dubine 3) brise sve regularne datoteke cija je velicina
 * veca od 10KB. Za brisanje datoteka koristi rm komandu Linux operativnog sistema.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_NAME 1024

void processdir(char *path, int level)
{
    printf("Procesuiram direktorijum: %s, nivo: %d:\n", path, level);
    
    if(level > 3)
    {
        printf("Dostigo sam maksimalnu dubinu: (nivo %d). Izlazim...\n", level);
        return;
    }
    
    DIR *dp;
    struct dirent *dirp;
    char path1[MAX_NAME];
    struct stat statbuf;
    
    dp = opendir(path);
    
    if(dp == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        return;
    }
    
    while((dirp = readdir(dp)) != NULL)
    {
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        
        strcpy(path1, path);
        strcat(path1, "/");
        strcat(path1,dirp-> d_name);
        
        if(stat(path1, &statbuf) == -1)
        {
            printf("Doslo je do greske prilikom ocitavanja statusa zadate putanje!\n");
            continue;
        }
        
        if(S_ISREG(statbuf.st_mode) && statbuf.st_size > 10 * 1024)
        {
            printf("Pronasao sam datoteku: %s, velicine: %ld B\n", path1, statbuf.st_size);
            if(fork() == 0)
            {
                printf("Delete file: %s\n", path1);
                execlp("rm", "rm", path1, NULL);
            }
            else
            {
                wait(NULL);
            }
        }
        else if(S_ISDIR(statbuf.st_mode) && level < 3)
        {
            printf("Ulazim u direktorijum: %s\n", path1);
            processdir(path1, level + 1);
        }
    }
    closedir(dp);
}

int main(int argc, char *argv[])
{
    char dirname[MAX_NAME];
    char path[MAX_NAME];
    struct stat statbuf;
    DIR *dp;
    struct dirent *dirp;
    
    if(argc < 2)
    {
        printf("Nedovoljan broj argumenata. Pokrenuti program: %s: <putanja_do_direktorijuma>\n", argv[0]);
        exit(-1);
    }
    
    strcpy(dirname, argv[1]);
    
    if(stat(dirname,&statbuf) < 0)
    {
        printf("Doslo je do greske prilikom ocitavanja statusa zadate putanje!\n");
        exit(-1);
    }
    
    if(!S_ISDIR(statbuf.st_mode))
    {
        printf("Uneti argument ne predstavlja putanju do direktorijuma!\n");
        exit(-1);
    }
    processdir(dirname, 1);
    
    return 0;
}
