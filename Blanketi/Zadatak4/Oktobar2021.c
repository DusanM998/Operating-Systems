/*Linux program koji u zadatom direktorijumu (zadaje se kao ulazni argument programa) odredjuje i brise
 * pet najvecih regularnih datoteka. Za brisanje datoteka koristiti Linux komandu rm.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 1024
#define MAX_FILES 5

struct fileInfo
{
    char path[MAX_PATH];
    off_t size;
};

//F-ja koja uporedjuje velicinu fajlova za sortiranje
int compareFiles(const void *a, const void *b)
{
    struct fileInfo *fileA = (struct fileInfo *) a;
    struct fileInfo *fileB = (struct fileInfo *) b;
    
    return (fileB->size - fileA->size); //Sortira u opadajucem redosledu
}

void processdir(char *path, struct fileInfo largestFiles[], int *count)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    char fullPath[MAX_PATH];
    
    dp = opendir(path);
    
    if(dp == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        return;
    }
    
    while((dirp = readdir(dp)) != NULL)
    {
        //Preskace trenutni i roditeljski direktorijum
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        
        strcpy(fullPath, path);
        strcat(fullPath, "/");
        strcat(fullPath,dirp-> d_name);
        
        if(stat(fullPath, &statbuf) < 0)
        {
            printf("Doslo je do greske pri ocitavanju statusa zadate putanje!\n");
            continue;
        }
        
        //Ako je regularna datoteka
        if(S_ISREG(statbuf.st_mode))
        {
            //Ako imamo manje od 5 fajlova, samo dodamo novi fajl
            if(*count < MAX_FILES)
            {
                strcpy(largestFiles[*count].path, fullPath);
                largestFiles[*count].size = statbuf.st_size;
                (*count)++;
            }
            else if(statbuf.st_size > largestFiles[MAX_FILES - 1].size) //Ako vec imamo 5 fajla samo uporedjujemo sa najmanjim i zamenjujemo mesta
            {
                strcpy(largestFiles[MAX_FILES - 1].path, fullPath);
                largestFiles[MAX_FILES - 1].size = statbuf.st_size;
            }
            
            //Fajlovi se sortiraju svaki put, da se osigura da je poslednji fajl najmanji
            qsort(largestFiles, *count, sizeof(struct fileInfo), compareFiles);
        }
        
        //Ako se radi o direktorijumu rekurzivno se poziva f-ja
        if(S_ISDIR(statbuf.st_mode))
            processdir(fullPath,largestFiles, count);
        
    }
    closedir(dp);
}

int main(int argc, char* argv[])
{
    struct fileInfo largestFiles[MAX_FILES];
    int count = 0;
    
    if(argc != 2)
    {
        printf("Pokrenuti program: %s <direktorijum>\n", argv[0]);
        exit(-1);
    }
    
    
    for(int i=0; i < MAX_FILES;i ++)
        largestFiles[i].size = 0;
    
    processdir(argv[1], largestFiles, &count);
    
    //Brisemo 5 najvecih fajlova
    for(int i=0; i < count; i++)
    {
        printf("Brisanje datoteke: %s (velicina %ld): \n", largestFiles[i].path, largestFiles[i].size);
        if(fork() == 0)
        {
            execlp("rm", "rm", largestFiles[i].path, NULL);
            exit(0);
        }
        else
            wait(NULL);
    }
    
    return 0;
}
