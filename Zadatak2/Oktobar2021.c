/*Linux program koji oponasa program more. Ovaj program ocekuje dva argumenta komandne linije. 
 * Prvi argument je naziv programa koji treba da se startuje i ciji izlaz da prikazuje ekran po ekran. 
 * Drugi argument N je broj linija koje prikazuje na ekranu posle cega prikazuje poruku --MORE--, 
 * ceka pritisak bilo kog tastera i nakon toga prikazuje N narednih linija, itd*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 1024

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        fprintf(stderr, "Usage: %s <command> <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[2]); // Broj linija po ekranu
    if (N <= 0) 
    {
        fprintf(stderr, "Invalid number of lines: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    int pd[2];
    if (pipe(pd) < 0) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) 
    { 
        // Child process: izvršava zadati program i šalje izlaz preko pipe-a
        close(pd[0]); // Zatvaramo kraj za čitanje

        dup2(pd[1], STDOUT_FILENO); // Preusmeravamo stdout u pipe
        close(pd[1]); // Zatvaramo kraj za pisanje nakon dup2

        // Prosleđujemo sve argumente od argv[1] na dalje programu koji se izvršava
        char *args[argc];
        for (int i = 1; i < argc - 1; i++) 
        {
            args[i-1] = argv[i];
        }
        args[argc - 2] = NULL;

        execvp(args[0], args); // Pokrećemo zadati program
        perror("execvp");
        exit(EXIT_FAILURE);
    } 
    else 
    { 
        // Parent process: prikazuje izlaz programa ekran po ekran
        close(pd[1]); // Zatvaramo kraj za pisanje

        FILE *stream = fdopen(pd[0], "r");
        if (!stream) 
        {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LENGTH];
        int line_count = 0;

        while (fgets(line, sizeof(line), stream)) 
        {
            printf("%s", line); // Prikazujemo liniju na ekranu
            line_count++;

            if (line_count >= N) 
            {
                printf("--MORE--");
                getchar(); // Čekamo unos od korisnika da nastavimo
                line_count = 0;
            }
        }

        fclose(stream); // Zatvaramo stream nakon čitanja
        wait(NULL); // Čekamo da dete završi
    }

    return 0;
}
