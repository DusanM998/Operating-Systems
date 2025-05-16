/*Linux program pronadjiDatoteku koji radi rekurzivno pretrazivanje stabla zadatog direktorijuma u potrazi za zadatom datotekom
 * i stampa njenu lokaciju i velicinu. Argumenti komandne linije programa su direktorijum koji se pretrazuje i naziv datoteke koja se trazi.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_NAME 1024

void pronadjiDatoteku(char *folderName, char *datoteka)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    int result;
    
    if((dp = opendir(folderName)) == NULL)
    {
        printf("Greska prilikom otvaranja foldera: %s!\n", folderName);
        return;
    }
    
    while((dirp = readdir(dp)) != NULL)
    {
        if(strcmp(dirp->d_name,".") == 0 || strcmp(dirp->d_name,"..") == 0)
            continue;
        
        char tmp[MAX_NAME] = "";
        strcat(tmp, folderName);
        strcat(tmp, "/");
        strcat(tmp, dirp->d_name);
        
        //Moze i snprintf(tmp, sizeof(tmp), "%s/%s", foldername, dirp->d_name); umesto dela od 32-35 linije
        
        printf("Obradjujem: %s u direktorijumu %s.\n", dirp->d_name, tmp);
        
        if((result = stat(tmp, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podatka o objektu %s!\n", tmp);
            continue;
        }
        
        //Ako je regularna datoteka i ako njeno ime odgovara fajlu koji se trazi
        if(S_ISREG(statbuf.st_mode) && strstr(dirp->d_name, datoteka) != NULL)
        {
            printf("******Pronasao sam datoteku na lokaciji: %s\n", tmp);
            printf("******Velicina datoteke: %s: %ld B\n", tmp, statbuf.st_size);
        }
        
        //Ako je direktorijum, rekurzivno se pretrazuje
        if(S_ISDIR(statbuf.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            printf("Otvaram folder: %s...\n", tmp);
            pronadjiDatoteku(tmp, datoteka);
        }
        
    }
    
    closedir(dp);
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Pokrenuti program u formatu: %s <direktorijum> <trazena_datoteka>\n", argv[0]);
        exit(1);
    }
    
    pronadjiDatoteku(argv[1], argv[2]);
    
}
