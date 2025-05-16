/*Linux program koji kreira dodatni proces sa kojim komunicira koriscenjem datavoda. Roditeljski proces generise
 * 20 slucajnih brojeva u opsegu od 100 do 199 i salje ih putem datavoda procesu detetu. Proces dete na ekranu
 * treba da odstampa samo brojeve koje dobije preko datavoda i koji su deljivi sa 3. U slucaju broja koji nije deljiv sa 3
 * ne stampa nista*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_RANDOMS 20
#define MIN_VALUE 100
#define MAX_VALUE 199

int main()
{
    int pd[2];
    int random_numbers[NUM_RANDOMS];
    
    //Kreiranje datavoda
    if(pipe(pd) == -1)
    {
        printf("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }
    
    //Kreiranje procesa
    int pid = fork();
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    if(pid == 0) //Proces dete
    {
        close(pd[1]);
        
        int broj;
        while(read(pd[0], &broj, sizeof(int)) > 0)
        {
            if(broj % 3 == 0)
            {
                printf("\nProces dete: Od procesa roditelja sam dobio broj: %d\n", broj);
                printf("Broj koji sam dobio od roditelja deljiv je sa 3.\n");
            }
            
        }
        
        close(pd[0]);
        exit(0);
    }
    else //Proces roditelj
    {
        close(pd[0]);
        
        srand(time(NULL));
        
        //Generise 20 slucajnih brojeva od 100 do 199
        for(int i=0; i < NUM_RANDOMS; i++)
        {
            random_numbers[i] = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
            write(pd[1], &random_numbers[i], sizeof(int));
        }
        close(pd[1]);
        wait(NULL);
    }
    
    return 0;
}
