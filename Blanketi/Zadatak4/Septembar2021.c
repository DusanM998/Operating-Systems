/*Linux program koji u zadatom direktorijumu (kao ulazni argument programa zadaje se apsolutna putanja do direktorijuma)
 * i njegovim poddirektorijumima(maksimalno do nivoa koji se zadaje kao ulazni argument programa, ukoliko se za nivo zadaje 0 onda se 
 * stablo direktorijuma obilazi po citavoj dubini) odredjuje i stampa prvih 10 linija iz svake regularne datoteke.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 1024
#define MAX_LINES 10

// Funkcija za ispis prvih 10 linija iz fajla
void printFirst10Lines(const char *filePath) 
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL) 
    {
        perror("Greska prilikom otvaranja fajla");
        return;
    }

    char line[MAX_PATH];
    int lineCount = 0;

    printf("***********Prvih 10 linija iz fajla: %s\n", filePath);
    while (fgets(line, sizeof(line), file) != NULL && lineCount < MAX_LINES) 
    {
        printf("%s", line);
        lineCount++;
    }

    fclose(file);
}

// Funkcija za rekurzivno obilazenje direktorijuma
void processDir(const char *path, int currentLevel, int maxLevel) 
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

        // Ako je regularna datoteka, ispisuje prvih 10 linija
        if (S_ISREG(statbuf.st_mode)) {
            printFirst10Lines(fullPath);
        }

        // Ako je direktorijum i nismo dostigli maksimalni nivo, rekurzivno ga procesuiramo
        if (S_ISDIR(statbuf.st_mode)) 
        {
            if (maxLevel == 0 || currentLevel < maxLevel) 
            {
                printf("Ulazim u direktorijum: %s (nivo %d)\n", fullPath, currentLevel);
                processDir(fullPath, currentLevel + 1, maxLevel);
            }
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Koriscenje: %s <putanja do direktorijuma> <maksimalni nivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *dirPath = argv[1];
    int maxLevel = atoi(argv[2]);

    // Provera da li je uneta putanja direktorijum
    struct stat statbuf;
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

    // Pokreni procesiranje direktorijuma sa početnim nivoom 1
    processDir(dirPath, 1, maxLevel);

    return 0;
}
