/*Linux program koji u zadatom direktorijumu(kao ulazni argument programa zadaje se apsolutna putanja do
 * direktorijuma) i rekurzivno u svim njegovim poddirektorijumima odredjuje sve regularne datoteke cija je 
 * velicina veca od N bajtova i cija je apsolutna putanja duza od M karaktera. N i M se takodje zadaju kao 
 * ulazni argumenti programa. Program na standardnom izlazu stampa apsolutne putanje do datoteka i njihove
 * velicine u bajtovima.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 1024

// Rekurzivna funkcija za pretragu direktorijuma
void processDir(const char *path, long N, int M) 
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

        // Ako je regularna datoteka, proveri velicinu i duzinu putanje
        if (S_ISREG(statbuf.st_mode)) 
        {
            if (statbuf.st_size > N && strlen(fullPath) > M) 
            {
                printf("Datoteka: %s, Velicina: %ld bajta\n", fullPath, statbuf.st_size);
            }
        }

        // Ako je direktorijum, rekurzivno ga procesuiraj
        if (S_ISDIR(statbuf.st_mode)) 
        {
            processDir(fullPath, N, M);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Koriscenje: %s <putanja do direktorijuma> <minimalna velicina N u bajtovima> <minimalna duzina M karaktera>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *dirPath = argv[1];
    long N = atol(argv[2]);  // Minimalna velicina datoteke u bajtovima
    int M = atoi(argv[3]);   // Minimalna duzina putanje

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

    // Procesiraj direktorijum
    processDir(dirPath, N, M);

    return 0;
}
