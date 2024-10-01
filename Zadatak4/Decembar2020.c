/*Linux program koji u zadatom direktorijumu(kao ulazni argument programa zadaje se apsolutna putanja do direktorijuma) 
 * i njegovim poddirektorijumima odredjuje imena svih regularnih datoteka koje su vece od 100KB, a manja od 750KB.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 1024
#define MIN_SIZE 102400 //100KB u bajtovima
#define MAX_SIZE 750000 //750KB u bajtovima

//Struktura koja predstavlja datoteku sa imenom i velicinom
typedef struct
{
    char path[MAX_PATH];
    off_t size;
} FileInfo;

//F-ja za poredjenje datoteka po velicini(za qsort)
int compareFiles(const void *a, const void *b)
{
    FileInfo *fileA = (FileInfo *)a;
    FileInfo *fileB = (FileInfo *)b;
    return (fileA->size - fileB->size);
}

//Rekurzivna f-ja za pretragu direktorijuma
void processDir(const char *path, FileInfo *files, int *fileCount)
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
        //Preskace "." i ".."
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        
        strcpy(fullPath, path);
        strcat(fullPath, "/");
        strcat(fullPath, dirp->d_name);
        //moze i snprintf(fullPath, sizeof(fullPath), "%s/%s", path, dirp->d_name);
        
        if(stat(fullPath, &statbuf) < 0)
        {
            printf("Doslo je do greske prilikom ocitavanja statusa putanje!\n");
            continue;
        }
        
        //Ako je regularna datoteka i veca od 100KB, a manje od 750KB
        if(S_ISREG(statbuf.st_mode) && statbuf.st_size > MIN_SIZE && statbuf.st_size < MAX_SIZE)
        {
            strcpy(files[*fileCount].path, fullPath);
            files[*fileCount].size = statbuf.st_size;
            (*fileCount)++;
        }
        
        //Ako je direktorijum, rekurzivno ga pretrazujemo
        if(S_ISDIR(statbuf.st_mode))
        {
            processDir(fullPath, files, fileCount);
        }
    }
    closedir(dp);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Pogresno uneti argumenti. Pokrenuti: %s <putanja_do_direktorijuma>\n", argv[0]);
        exit(-1);
    }
    
    char *dirPath = argv[1];
    //strcpy(dirPath, argv[1]);
    struct stat statbuf;
    
    if(stat(dirPath, &statbuf) < 0)
    {
        printf("Doslo je do greske prilikom ocitavanja statusa putanje!\n");
        exit(-1);
    }
    
    if(!S_ISDIR(statbuf.st_mode))
    {
        fprintf(stderr, "Uneta putanja ne predstavlja direktorijum!\n");
        exit(-1);
    }
    
    //Alokacija memorije za datoteke
    FileInfo files[1000];
    int fileCount = 0;
    
    //Procesuiramo direktorijum
    processDir(dirPath, files, &fileCount);
    
    //Sortira datoteke po velicini u rastucem redosledu
    qsort(files, fileCount, sizeof(FileInfo), compareFiles);
    
    //Ispisuje imena sortiranih datoteka
    printf("Regularne datoteke vece od 100KB i manje od 750KB: \n");
    for(int i=0; i < fileCount; i++)
    {
        printf("Datoteka: %s | velicine: %ld B\n", files[i].path, files[i].size);
    }
    
    return 0;
}
