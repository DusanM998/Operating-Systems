/*Linux program koji kao ulazne argumente prihvata apsolutnu putanju do nekog direktorijuma i parametre n i m.
 * Program ima zadatak da u zadatom direktorijumu iz n prvih regularnih datoteka odstampa m linija sa pocetka datoteke
 * (pretpostaviti da je maximalna duzina linije u datoteci 100 karaktera).*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 1024
#define MAX_LINE_LEN 100

// Funkcija za štampanje m linija iz datoteke
void print_first_m_lines(char *filepath, int m) 
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL) 
    {
        printf("Ne mogu otvoriti datoteku: %s\n", filepath);
        return;
    }

    char line[MAX_LINE_LEN];
    int line_count = 0;

    // Čitamo i štampamo m linija
    printf("***********Prvih %d linija iz fajla: %s\n", m, filepath);
    while (fgets(line, MAX_LINE_LEN, file) != NULL && line_count < m) 
    {
        printf("%s", line);
        line_count++;
    }

    fclose(file);
}

void processDir(char *path, int *file_count, int n, int m)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    char path1[MAX_PATH];
    
    if((dp = opendir(path)) == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja direktorijuma!\n");
        return;
    }
    
    while((dirp = readdir(dp)) != NULL && *file_count < n)
    {
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        
        strcpy(path1, path);
        strcat(path1, "/");
        strcat(path1, dirp->d_name);
        
        if(stat(path1, &statbuf) < 0)
        {
            printf("Doslo je do greske prilikom ocitavanja statusa zadate putanje!\n");
            continue;
        }
        
        if(S_ISREG(statbuf.st_mode))
        {
            printf("\n***********Procesuiram datoteku: %s***********\n", path1);
            print_first_m_lines(path1, m);
            (*file_count)++;
        }
        else if(S_ISDIR(statbuf.st_mode))
            processDir(path1, file_count, n, m);
        
    }
    
    closedir(dp);
}
    
int main(int argc, char *argv[]) 
{
    if (argc < 4) 
    {
        printf("Nema dovoljno ulaznih argumenata\n");
        printf("Koriscenje: %s <direktorijum> <n> <m>\n", argv[0]);
        exit(-1);
    }

    char dirname[MAX_PATH];
    strcpy(dirname, argv[1]);
    int n = atoi(argv[2]); // Broj datoteka
    int m = atoi(argv[3]); // Broj linija iz svake datoteke
    
    struct stat statbuf;
    int file_count = 0;

    // Provera da li je zadata putanja validna
    if (stat(dirname, &statbuf) < 0) 
    {
        printf("Doslo je do greske prilikom ocitavanja statusa zadate putanje\n");
        exit(-1);
    }

    // Proveravamo da li je uneti argument direktorijum
    if (!S_ISDIR(statbuf.st_mode)) 
    {
        printf("Uneti argument ne predstavlja putanju do direktorijuma\n");
        exit(-1);
    }

    processDir(dirname, &file_count, n, m);

    // Ako nema dovoljno datoteka u direktorijumu
    if (file_count < n) 
    {
        printf("\nU direktorijumu ima manje od %d regularnih datoteka.\n", n);
    }

    return 0;
}
