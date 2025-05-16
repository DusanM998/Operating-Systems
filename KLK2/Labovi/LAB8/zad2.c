/*Linux program koji prikazuje statistuku da zadati direktorijum( broj poddirektorijuma, regularnih datoteka i linkova). Putanja do direktorijuma(apsolutna ili relativna) se zadaje kao argument komandne linije*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    DIR* dp;
    struct dirent* dirp;
    int brfldr, brdat, brlnk;
    struct stat statbuf;
    int result;
    char path[255];
    
    if(argc != 2)
    {
        printf("Neophodan je jedan argument (putanja do direktorijuma)\n");
        exit(1);
    }
    
    if((dp = opendir(argv[1])) == NULL)
    {
        printf("Greska prilikom otvaranja foldera %s\n", argv[1]);
        exit(1);
    }
    
    brfldr = brdat = brlnk = 0;
    
    while( (dirp = readdir(dp)) != NULL)
    {
        //formiranje putanje do datoteke
        strcpy(path, argv[1]);
        strcat(path, "/");
        strcat(path, dirp->d_name);
        
        printf("Test: %s\n", path);
        
        /*Stat struktura cuva informacije o datoteci*/
        if((result = stat(path, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu %s\n", dirp->d_name);
            continue;
        }
        if(S_ISDIR(statbuf.st_mode))
        {
            printf("Objekat: %s je regularna datoteka!\n", dirp->d_name);
            brfldr++;
        }
        if(S_ISREG(statbuf.st_mode))
        {
            printf("Objekat: %s je link\n", dirp->d_name);
            brdat++;
        }
        if(S_ISLNK(statbuf.st_mode))
        {
            printf("Objekat: %s je link\n", dirp->d_name);
            brlnk++;
        }
    }
    
    printf("Statistika za folder: %s\n Podfoldera: %d\n Reg. datoteka: %d\n Linkova: %d\n", argv[1], brfldr, brdat, brlnk);
    
    closedir(dp);
}
