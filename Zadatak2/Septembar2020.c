/*Linux program koji omogucava modifikaciju standardnog izlaza bilo kog programa i snimanje standardnog izlaza
 * u specificiranu datoteku. Naziv programa koji se startuje i ciji se izlaz modifikuje se zadaje kao prvi argument
 * komandne linije, a naziv tekstualne datoteke u koju se upisuje izlaz kao drugi argument komandne linije.
 * Standardni izlaz se modifikuje tako da se svako pojavljivanje karaktera "b" zamenjuje karakterom "B".*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) 
{
    int pd[2];
    int pid;
    
    if (argc < 3) 
    {
        fprintf(stderr, "Usage: %s <program> <output_file>\n", argv[0]);
        exit(1);
    }

    

    if (pipe(pd) < 0) 
    {
        printf("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) 
    {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }

    if (pid == 0) 
    { 
        // Child proces koji izvršava zadati program
        close(pd[0]); // Zatvaramo kraj za čitanje
        dup2(pd[1], 1); // Preusmeravamo stdout u pipe
        close(pd[1]); // Zatvaramo kraj za pisanje nakon dup2

        execlp(argv[1], argv[1], NULL); // Pokrećemo zadati program
        perror("execlp"); // Ako dođe do greške
        exit(EXIT_FAILURE);
    } 
    else 
    { 
        // Parent proces koji modifikuje i zapisuje izlaz u datoteku
        close(pd[1]); // Zatvaramo kraj za pisanje

        FILE *output_file = fopen(argv[2], "w");
        if (output_file == NULL) 
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char buffer;
        while (read(pd[0], &buffer, 1) > 0) 
        {
            if (buffer == 'b') 
            {
                buffer = 'B';
            }
            fputc(buffer, output_file); // Pišemo modifikovani karakter u datoteku
        }

        close(pd[0]); // Zatvaramo kraj za čitanje
        fclose(output_file); // Zatvaramo datoteku
        wait(NULL); // Čekamo da proces dete završi
    }

    return 0;
}
