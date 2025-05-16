/*Linux program koji kreira dodatni proces dete. Proces roditelj sa tastature ucitava putanju do tekstualne datoteke
 * i jednu kljucnu rec. Ova dva procesa komuniciraju dvosmerno koriscenjem datavoda. Proces roditelj koriscenjem datavoda
 * procesu detetu salje putanju do tekstualne datoteke i jednu kljucnu rec. Proces dete otvara tu tekstualnu datoteku
 * i trazi u kojim linijama se javlja kljucna rec. Proces dete procesu roditelju koriscenjem datavoda vraca redne brojeve 
 * linija u kojima se javlja kljucna rec. Proces roditelj na ekranu stampa primljene brojeve linija.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_KEYWORD_LEN 100
#define MAX_LINE_LEN 1024
#define MAX_PATH_LEN 256

int main()
{
    int pipe_roditelj_detetu[2];
    int pipe_dete_roditelju[2];
    int pid;
    
    if(pipe(pipe_roditelj_detetu) < 0 || pipe(pipe_dete_roditelju) < 0)
    {
        perror("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }
    
    pid = fork();
    if(pid < 0)
    {
        perror("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    if(pid == 0) 
    {
        //Proces dete: od roditelja prima preko datavoda putanju do tekstualne datoteke
        //i jednu kljucnu rec, pronalazi u kojim se linijama javlja kljucna rec i vraca broj linije roditelju
        close(pipe_roditelj_detetu[1]);
        close(pipe_dete_roditelju[0]);
        
        char filepath[MAX_PATH_LEN];
        char keyword[MAX_KEYWORD_LEN];
        
        read(pipe_roditelj_detetu[0], filepath, MAX_PATH_LEN); //Cita sa datavoda putanju do txt. dat. od roditelja
        read(pipe_roditelj_detetu[0], keyword, MAX_KEYWORD_LEN); //Cita sa datavoda kljucnu rec od roditelja
        
        FILE *file = fopen(filepath, "r");
        if(file == NULL)
        {
            perror("Greska prilikom otvaranja datoteke!\n");
            exit(1);
        }
        
        char line[MAX_LINE_LEN];
        int line_number = 0;
        int found = 0;
        
        while(fgets(line, sizeof(line), file))
        {
            line_number++;
            if(strstr(line, keyword) != NULL)
            {
                printf("Proces dete: Pronasao sam kljucnu rec %s u liniji %d\n", keyword, line_number);
                printf("Proces dete: Saljem roditelju redni broj linija...\n");
                found = 1;
                write(pipe_dete_roditelju[1], &line_number, sizeof(line_number));
            }
        }
        if(!found)
        {
            printf("Proces dete: Kljucna rec: %s nije pronadjena u datoteci <%s>!\n", keyword, filepath);
        }
        fclose(file);
        close(pipe_roditelj_detetu[0]);
        close(pipe_dete_roditelju[1]);
        exit(0);
    }
    else //Proces roditelj: Sa tastature ucitava putanju do tekstualne datoteke i jednu kljucnu rec
    {
        close(pipe_roditelj_detetu[0]);
        close(pipe_dete_roditelju[1]);
        
        char filepath[MAX_PATH_LEN];
        char keyword[MAX_KEYWORD_LEN];
        
        printf("Unesite putanju do tekstualne datoteke: ");
        fgets(filepath, MAX_PATH_LEN, stdin);
        filepath[strcspn(filepath, " \n")] = '\0';
        
        printf("Unesite kljucnu rec: ");
        fgets(keyword, MAX_KEYWORD_LEN, stdin);
        keyword[strcspn(keyword, " \n")] = '\0';
        
        write(pipe_roditelj_detetu[1], filepath, MAX_PATH_LEN);
        write(pipe_roditelj_detetu[1], keyword, MAX_KEYWORD_LEN);
        
        int line_number = 0;
        printf("Proces roditelj: Redni brojevi linija u kojima je kljucna rec <%s>:\n", keyword);
        while(read(pipe_dete_roditelju[0], &line_number, sizeof(line_number)) > 0)
        {
            if (line_number == -1) 
            {
                printf("Ključna reč nije pronađena u datoteci.\n");
                break;
            }
            else
            {
                printf("Linija: %d\n", line_number);
            }
        }
        
        close(pipe_roditelj_detetu[1]);
        close(pipe_dete_roditelju[0]);
        
        wait(NULL);
        exit(0);
    }
    
    return 0;
}
