/*Linux program koji kreira dodatni proces dete. Proces roditelj sa tastature ucitava putanju do tekstualne datoteke i jednu kljucnu rec.
 * Ova dva procesa komuniciraju koriscenjem datavoda od procesa roditelja ka procesu detetu. Proces roditelj koriscenjem datavoda procesu detetu
 * salje tu putanju do tekstualne datoteke i kljucnu rec. Proces dete otvara tu tekstualnu datoteku i trazi u kojim linijama se javlja kljucna rec.
 * Proces dete na ekranu stampa brojeve linija u kojima se javlja kljucna rec.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_PATH_LENGTH 256
#define MAX_KEYWORD_LENGTH 100
#define MAX_LINE_LENGTH 1024

int main() 
{
    int pd[2];
    
    // Kreiranje datavoda
    if (pipe(pd) == -1) 
    {
        perror("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }

    // Kreiranje procesa deteta
    int pid = fork();
    if (pid < 0) 
    {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) 
    { 
        // Proces dete: od procesa roditelja prima putanju do tekstualne datoteke i kljucnu rec
        //Zatim na ekranu stampa brojeve linija u kojima se javlja kljucna rec
        close(pd[1]); // Zatvori kraj za pisanje u datavodu

        char filepath[MAX_PATH_LENGTH];
        char keyword[MAX_KEYWORD_LENGTH];

        // Čitanje putanje i ključne reči iz datavoda
        read(pd[0], filepath, MAX_PATH_LENGTH);
        read(pd[0], keyword, MAX_KEYWORD_LENGTH);

        // Otvaranje datoteke
        FILE *file = fopen(filepath, "r");
        if (file == NULL) 
        {
            perror("Greska prilikom otvaranja fajla!\n");
            exit(1);
        }

        char line[MAX_LINE_LENGTH];
        int line_number = 0;
        int found = 0;

        // Traženje ključne reči u datoteci
        //fgets cita red iz datoteke (1. arg. linija koju citamo.
        //2. arg. max br. karaktera koji ce biti procitani,
        //3. arg. datoteka iz koje citamo
        while (fgets(line, sizeof(line), file)) 
        {
            line_number++;
            //F-ja strstr trazi prvo pojavljivanje podniza unutar nekog niza(1. arg. niz u kome se trazi, 2. arg. podniz)
            if (strstr(line, keyword) != NULL) 
            {
                printf("Kljucna rec '%s' pronađena u liniji %d\n", keyword, line_number);
                found = 1;
            }
        }

        if (!found) 
        {
            printf("Kljucna rec '%s' nije pronađena u datoteci <%s>.\n", keyword, filepath);
        }

        fclose(file);
        close(pd[0]); // Zatvori kraj za čitanje u datavodu
        exit(0);
    } 
    else 
    { 
        //Proces roditelj: procesu detetu salje putanju do tekstualne datoteke i kljucnu rec
        close(pd[0]); // Zatvori kraj za čitanje u datavodu

        char filepath[MAX_PATH_LENGTH];
        char keyword[MAX_KEYWORD_LENGTH];

        // Unos putanje do datoteke i ključne reči
        printf("Unesite putanju do tekstualne datoteke: ");
        fgets(filepath, MAX_PATH_LENGTH, stdin);
        filepath[strcspn(filepath, "\n")] = '\0'; // Uklanjanje novog reda

        printf("Unesite ključnu reč: ");
        fgets(keyword, MAX_KEYWORD_LENGTH, stdin);
        keyword[strcspn(keyword, "\n")] = '\0'; // Uklanjanje novog reda

        // Slanje putanje i ključne reči procesu detetu preko datavoda
        write(pd[1], filepath, MAX_PATH_LENGTH);
        write(pd[1], keyword, MAX_KEYWORD_LENGTH);

        close(pd[1]); // Zatvori kraj za pisanje u datavodu
        wait(NULL); // Čekanje da se proces dete završi
    }

    return 0;
}
