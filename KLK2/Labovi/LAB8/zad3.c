/*Linux program koji u zadatom direktorijumu(naziv direktorijuma se zadaje kao 1. argument komandne linije) pronalazi i stampa nazive svih datoteka koje u imenu sadrze string koji se zadaje kao 2. argument komandne linije. Pretrazivanje se obavlja rekurzivno u zadatom direktorijumu i svim njegovi poddirektorijumima.*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void processdir(char* folderName, char* str)
{
    DIR* dp;
    struct dirent* dirp;
    
    struct stat statbuf;
    int result;
    
    if((dp = opendir(folderName)) == NULL)
    {
        printf("Greska prilikom otvaranja foldera: %s!\n", folderName);
        return;
    }
    
    while( (dirp = readdir(dp)) != NULL)
    {
        char tmp[1024] = "";
        strcat(tmp, folderName);
        strcat(tmp, "/");
        strcat(tmp, dirp->d_name);
        
        printf("Obradjujem: %s u %s.\n", dirp->d_name, tmp);
        
        if((result = stat(tmp, &statbuf)) == -1)
        {
            printf("Neuspenso citanje podataka o objektu: %s!\n", tmp);
            continue;
        }
        
        if(strstr(dirp->d_name, str) != NULL)
            printf("********Pronasao %s\n", tmp);
        
        if(S_ISDIR(statbuf.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            printf("Otvaram folder %s.\n", tmp);
            processdir(tmp, str);
        }
    }
    
    closedir(dp);
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Neophodna su dva argumenta (putanja do foldera i string koji se trazi!\n");
        exit(1);
    }
    
    processdir(argv[1], argv[2]);
}
