/*Koriscenjem prog. jez c kreirati tri Linux procesa koja komuniciraju koriscenjem datavoda. Prvi proces generise niz proizvoljnih karaktera (duzina niza je fiksna i iznosi 20 karaktera), 
 *a zatim racuna zbir ASCII vrednosti svih tako generisanih karaktera. Ukoliko je dobijeni zbir paran broj, 
 * generisani niz karaktera se koriscenjem datavoda salje drugom procesu koji ih upisuje u datoteku KARAKTERI.txt. 
 * Ukoliko je dobijeni zbir neparan broj, generisani niz karaktera se koriscenjem datavoda salje trecem procesu koji ih stampa na standardnom izlazu.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DUZINA_NIZA 20
#define FAJL "KARAKTERI.txt"

// Funkcija za generisanje slučajnog karaktera
char generisi_karakter() 
{
    int random_char = rand() % 52; // Ukupno 52 slova (26 velikih + 26 malih)
    if (random_char < 26) 
    {
        return 'A' + random_char; // Generišemo veliko slovo
    } 
    else 
    {
        return 'a' + (random_char - 26); // Generišemo malo slovo
    }
}

// Funkcija za generisanje niza karaktera
void generisi_niz(char *niz) 
{
    for (int i = 0; i < DUZINA_NIZA; i++) 
    {
        niz[i] = generisi_karakter(); // Poziv funkcije koja generiše pojedinačni karakter
    }
    niz[DUZINA_NIZA] = '\0'; // Završetak stringa
}

// Funkcija za računanje zbira ASCII vrednosti
int racunaj_zbir(char *niz) 
{
    int zbir = 0;
    for (int i = 0; i < DUZINA_NIZA; i++) 
    {
        zbir += (int)niz[i]; // Sabiramo ASCII vrednosti karaktera
    }
    return zbir;
}

int main() 
{
    int pipefd[2];
    pid_t pid1, pid2, pid3;

    // Kreiranje datavoda
    if (pipe(pipefd) == -1) 
    {
        perror("Greska prilikom kreiranja datavoda");
        exit(1);
    }

    srand(time(NULL)); // Inicijalizacija generatora slučajnih brojeva

    pid1 = fork();
    if (pid1 < 0) 
    {
        perror("Greska prilikom kreiranja procesa 1");
        exit(1);
    }

    if (pid1 == 0) 
    {
        // Proces 1: Generiše niz i računa zbir
        close(pipefd[0]); // Zatvaramo kraj za čitanje u datavodu

        char niz[DUZINA_NIZA + 1];
        generisi_niz(niz);
        int zbir = racunaj_zbir(niz);

        if (zbir % 2 == 0) 
        {
            // Zbir je paran, šaljemo niz procesu 2
            if (write(pipefd[1], niz, strlen(niz) + 1) == -1) 
            {
                perror("Greska prilikom slanja podataka procesu 2");
                exit(1);
            }
        } 
        else 
        {
            // Zbir je neparan, šaljemo niz procesu 3
            if (write(pipefd[1], niz, strlen(niz) + 1) == -1) 
            {
                perror("Greska prilikom slanja podataka procesu 3");
                exit(1);
            }
        }

        close(pipefd[1]); // Zatvaramo kraj za pisanje u datavodu
        exit(0);
    }

    pid2 = fork();
    if (pid2 < 0) 
    {
        perror("Greska prilikom kreiranja procesa 2");
        exit(1);
    }

    if (pid2 == 0) 
    {
        // Proces 2: Prima niz i upisuje ga u datoteku ako je zbir paran
        close(pipefd[1]); // Zatvaramo kraj za pisanje u datavodu

        char niz[DUZINA_NIZA + 1];
        int fd = open(FAJL, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) 
        {
            perror("Greska prilikom otvaranja datoteke");
            exit(1);
        }

        if (read(pipefd[0], niz, sizeof(niz)) > 0) 
        {
            if (write(fd, niz, strlen(niz)) == -1 || write(fd, "\n", 1) == -1) 
            {
                perror("Greska prilikom pisanja u datoteku");
            }
        }

        close(fd); // Zatvaramo fajl
        close(pipefd[0]); // Zatvaramo kraj za čitanje u datavodu
        exit(0);
    }

    pid3 = fork();
    if (pid3 < 0) 
    {
        perror("Greska prilikom kreiranja procesa 3");
        exit(1);
    }

    if (pid3 == 0) 
    {
        // Proces 3: Prima niz i ispisuje ga na standardni izlaz ako je zbir neparan
        close(pipefd[1]); // Zatvaramo kraj za pisanje u datavodu

        char niz[DUZINA_NIZA + 1];
        if (read(pipefd[0], niz, sizeof(niz)) > 0) 
        {
            printf("Proces 3: Primljen niz: %s\n", niz);
        }

        close(pipefd[0]); // Zatvaramo kraj za čitanje u datavodu
        exit(0);
    }

    // Roditeljski proces
    close(pipefd[0]); // Zatvaramo kraj za čitanje u datavodu
    close(pipefd[1]); // Zatvaramo kraj za pisanje u datavodu

    // Čekamo da se svi procesi završe
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
