/*Linux program kome se kao argumenti komandne linije prosledjuju naziv tekstualne datoteke i proizvoljan broj reci.
 * Zatim glavni program kreira dva dodatna procesa. Ova tri procesa su povezana datavodima. Glavni proces naziv tekstualne datoteke
 * i kljucne reci prosledjuje datavodom procesu detetu. Prvi proces dete u toj tekstualnoj datoteci pronalazi redne brojeve linije
 * u kojima se nalaze prosledjene kljucne reci. Kako koje pojavljivanje kljucne reci pronadje redni broj linije salje datavodom trecem procesu.
 * Treci proces sabira dobijene redne brojeve linija i kada su sva pojavljivanja svih kljucnih reci pronadjena, treci proces na ekranu stampa
 * zbir njemu proslednjenih rednih brojeva linija.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF 1024

void close_pipes(int pipes[2]) 
{
    close(pipes[0]);
    close(pipes[1]);
}

int main(int argc, char *argv[]) 
{
    
    char filename[MAX_BUF];
    int num_words = argc - 2;
    char words[num_words][MAX_BUF];
    int pipe_roditelj_detetu[2], pipe_dete_roditelju[2];
    int pid1;
    
    if (argc < 3) 
    {
        printf("Lose uneti parametri. Uneti ih u obliku: %s <file_name> <word1> <word2> ...\n", argv[0]);
        exit(1);
    }
    
    if (pipe(pipe_roditelj_detetu) == -1 || pipe(pipe_dete_roditelju) == -1) 
    {
        perror("Greska prilikom kreiranja datavoda\n");
        exit(1);
    }

    pid1 = fork();
    if (pid1 < 0) 
    {
        perror("Greska prilikom kreiranja novog procesa!\n");
        exit(1);
    }

    if (pid1 == 0) 
    { 
        //Prvo dete: u tekstualnoj datoteci ciju putanju dobije od roditelja, kao i kljucnu rec
        //pronalazi redne brojeve linije u kojima se nalaze prosledjene kljucne reci
        
        close(pipe_roditelj_detetu[1]); // Zatvara stranu za pisanje prvog datavoda
        close(pipe_dete_roditelju[0]); // Zatvara stranu za citanje drugog datavoda

        read(pipe_roditelj_detetu[0], filename, MAX_BUF); //od glavnog procesa dobija putanju do tekstualne datoteke
        for (int i = 0; i < num_words; i++) 
        {
            read(pipe_roditelj_detetu[0], words[i], MAX_BUF); //od glavnog procesa dobija prosledjene reci
        }

        FILE *file = fopen(filename, "r");
        if (!file) 
        {
            perror("Greska prilikom otvaranja fajla!\n");
            close_pipes(pipe_roditelj_detetu);
            close_pipes(pipe_dete_roditelju);
            exit(1);
        }

        char line[MAX_BUF];
        int line_number = 0;
        while (fgets(line, MAX_BUF, file)) 
        {
            line_number++;
            for (int i = 0; i < num_words; i++) 
            {
                if (strstr(line, words[i])) 
                {
                    write(pipe_dete_roditelju[1], &line_number, sizeof(int));
                }
            }
        }

        fclose(file);
        close_pipes(pipe_roditelj_detetu);
        close(pipe_dete_roditelju[1]); // Close writing end of pipe2
        exit(0);
    }

    int pid2 = fork();
    if (pid2 < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid2 == 0) 
    { // Child process 2
        close(pipe_roditelj_detetu[0]); // Close reading end of pipe1
        close(pipe_roditelj_detetu[1]); // Close writing end of pipe1
        close(pipe_dete_roditelju[1]); // Close writing end of pipe2

        int line_number, sum = 0;
        while (read(pipe_dete_roditelju[0], &line_number, sizeof(int)) > 0) 
        {
            sum += line_number;
        }

        printf("Suma rednih brojeva linija iz fajla: %d\n", sum);
        close(pipe_dete_roditelju[0]); // Close reading end of pipe2
        exit(0);
    }

    // Parent process
    close(pipe_roditelj_detetu[0]); // Close reading end of pipe1
    close(pipe_dete_roditelju[0]); // Close reading end of pipe2
    close(pipe_dete_roditelju[1]); // Close writing end of pipe2

    write(pipe_roditelj_detetu[1], argv[1], MAX_BUF);
    for (int i = 2; i < argc; i++) 
    {
        write(pipe_roditelj_detetu[1], argv[i], MAX_BUF);
    }

    close(pipe_roditelj_detetu[1]); // Close writing end of pipe1
    wait(NULL);
    wait(NULL);

    return 0;
}
