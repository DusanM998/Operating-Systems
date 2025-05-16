/*Linux program koji u zadatom direktorijumu(kao ulazni argument programa zadaje se apsolutna putanja do
 * direktorijuma) i rekurzivno u njegovim poddirektorijumima odredjuje i stampa imena svih regularnih datoteka
 * koje su vece od 100KB tako da je spisak sortiranih datoteka sortiran po velicini u rastucem redosledu.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 1024
#define MIN_SIZE 102400 // 100KB u bajtovima

// Struktura koja predstavlja datoteku sa imenom i velicinom
typedef struct 
{
    char path[MAX_PATH];
    off_t size;
} FileInfo;

// Funkcija za poređenje datoteka po veličini (za qsort)
int compareFiles(const void *a, const void *b) 
{
    FileInfo *fileA = (FileInfo *)a;
    FileInfo *fileB = (FileInfo *)b;
    return (fileA->size - fileB->size);
}

// Rekurzivna funkcija za pretragu direktorijuma
void processDir(const char *path, FileInfo *files, int *fileCount) 
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    char fullPath[MAX_PATH];

    dp = opendir(path);
    if (dp == NULL) 
    {
        perror("Greska prilikom otvaranja direktorijuma");
        return;
    }

    while ((dirp = readdir(dp)) != NULL) 
    {
        // Preskoci "." i ".."
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, dirp->d_name);

        if (stat(fullPath, &statbuf) == -1) 
        {
            perror("Stat greska");
            continue;
        }

        // Ako je regularna datoteka i veća od 100KB
        if (S_ISREG(statbuf.st_mode) && statbuf.st_size > MIN_SIZE) 
        {
            strcpy(files[*fileCount].path, fullPath);
            files[*fileCount].size = statbuf.st_size;
            (*fileCount)++;
        }

        // Ako je direktorijum, rekurzivno ga procesuiraj
        if (S_ISDIR(statbuf.st_mode)) 
        {
            processDir(fullPath, files, fileCount);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Koriscenje: %s <putanja do direktorijuma>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *dirPath = argv[1];
    struct stat statbuf;

    // Proveri da li je uneta putanja direktorijum
    if (stat(dirPath, &statbuf) == -1) 
    {
        perror("Greska prilikom ocitavanja statusa putanje");
        exit(EXIT_FAILURE);
    }

    if (!S_ISDIR(statbuf.st_mode)) 
    {
        fprintf(stderr, "Uneta putanja nije direktorijum\n");
        exit(EXIT_FAILURE);
    }

    // Alokacija memorije za datoteke
    FileInfo files[1000]; // Maksimalno 1000 datoteka
    int fileCount = 0;

    // Procesiranje direktorijuma
    processDir(dirPath, files, &fileCount);

    // Sortiraj datoteke po veličini
    qsort(files, fileCount, sizeof(FileInfo), compareFiles);

    // Ispisi sortirane datoteke
    printf("Regularne datoteke vece od 100KB:\n");
    for (int i = 0; i < fileCount; i++) 
    {
        printf("%s (velicina: %ld B)\n", files[i].path, files[i].size);
    }

    return 0;
}
