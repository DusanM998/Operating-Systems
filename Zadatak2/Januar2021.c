/*Linux program koji kreira dodatni proces sa kojim komunicira koriscenjem datavoda. Roditeljski proces generise 5
 * slucajnih celih pozitivnih brojeva u opsegu od 0 do 99 i salje ih putem datavoda procesu detetu. Proces dete, ukoliko je primljen broj paran
 * koriscenjem datavoda procesu roditelju vraca taj broj. U slucaju neparnog broja ne radi nista. 
 * Na kraju razmene svih 5 slucajno generisanih brojeva, proces roditelj na ekranu stampa koliko je bilo parnih brojeva.*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ITERATIONS 5
#define MAX_VALUE 99

int main()
{
    int pipe_roditelj_detetu[2];
    int pipe_dete_roditelju[2];
    int pid;
    int random_numbers[NUM_ITERATIONS];
    int brojParnih = 0;
    int result;
    
    //Kreiranje datavoda
    if(pipe(pipe_roditelj_detetu) == -1 || pipe(pipe_dete_roditelju) == -1)
    {
        printf("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }
    
    //Kreiranje dodatnog procesa(procesa deteta)
    pid = fork();
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    //proces dete izvrsava ovaj deo koda
    if(pid == 0)
    {
        close(pipe_roditelj_detetu[1]);
        close(pipe_dete_roditelju[0]);
        
        //Proces dete dobila od procesa roditelja generisani broj
        //...i vraca ga roditelju ako je broj paran
        int broj;
        while(read(pipe_roditelj_detetu[0], &broj, sizeof(int)) > 0)
        {
            if(broj % 2 == 0)
            {
                printf("Proces dete: Dobio sam broj: %d od roditelja, saljem mu nazad broj\n", broj);
                write(pipe_dete_roditelju[1], &broj, sizeof(int));
            }
            else
            {
                printf("Proces dete: Dobio sam broj %d koji je neparan, ne saljem ga roditelju.\n", broj);
            }

        }
        
        close(pipe_roditelj_detetu[0]);
        close(pipe_dete_roditelju[1]);
        exit(0);
    }
    else
    {
        //Deo koda koji izvrsava proces roditelj
        close(pipe_roditelj_detetu[0]);
        close(pipe_dete_roditelju[1]);
        
        srand(time(NULL));
        
        //Generisanje random brojeva i slanja detetu
        for(int i=0; i < NUM_ITERATIONS; i++)
        {
            random_numbers[i] = rand() % (MAX_VALUE + 1);
            printf("Proces roditelj: Generisao sam broj: %d i saljem ga detetu\n", random_numbers[i]);
            write(pipe_roditelj_detetu[1], &random_numbers[i], sizeof(int));
        }
        
        close(pipe_roditelj_detetu[1]);
        
        while(read(pipe_dete_roditelju[0], &result, sizeof(int)) > 0)
        {
            brojParnih++;
            printf("Proces roditelj: Primio sam od procesa deteta: %d\n", result);
        }
        
        close(pipe_dete_roditelju[0]);
        wait(NULL);
        
        printf("Proces roditelj: Ukupan broj parnih brojeva primljenih od procesa deteta: %d\n", brojParnih);
    }
    return 0;
}
