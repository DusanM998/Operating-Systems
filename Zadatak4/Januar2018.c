/*Linux program koji pocevsi od korenskog(root) direktorijima, pronalazi i stampa sve regularne datoteke
 * cija je apsolutna putanja duza od 256 karaktera i cija je velicina manja od 2KB.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define MAX_PATH 4096

void findFiles(const char *dirPath)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    char fullPath[MAX_PATH];
    
    //Otvaranje direktorijima
    if((dp = opendir(dirPath)) == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja direktorijima!\n");
        return;
    }
    
    //Citanje ulaza u direktorijim
    while((dirp = readdir(dp)) != NULL)
    {
        //Preskace trenutni "." i roditeljski direktorijim ".."
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        
        //Kreira putanju do trenutnog direktorijima
        strcpy(fullPath, dirPath);
        strcat(fullPath, "/");
        strcat(fullPath, dirp->d_name);
        
        //Pribavlja informacije o fajlu
        if(stat(fullPath, &statbuf) < 0)
        {
            printf("Doslo je do greske prilikom ocitavanja statusa zadate putanje!\n");
            continue;
        }
        
        //Proverava da li je regularan fajl, da li je velicina < 2KB i putanja duza od 256 karaktera
        if(S_ISREG(statbuf.st_mode) && statbuf.st_size < 2 * 1024 && strlen(fullPath) > 256)
        {
            printf("Pronadjena datoteka: %s | velicine: %ld, cija je putanja duza od 256 karaktera.\n", fullPath, statbuf.st_size);
        }
        
        //Ako je direktorijim, rekurzivno ga pretrazujemo
        if(S_ISDIR(statbuf.st_mode))
            findFiles(fullPath);
    }
    
    closedir(dp);
    
}

int main()
{
    //Startuje se pretraga pocevsi od root(/) direktorijima
    findFiles("/");
    
    return 0;
}
