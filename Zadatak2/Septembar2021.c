/*Linux program koji kreira dodatni proces sa kojim komunicira koriscenjem datavoda. Roditeljski proces generise 10
 * slucajnih brojeva u opsegu od 00 do 99 i salje ih putem datavoda procesu detetu. Proces dete na ekranu treba da 
 * odstampa samo brojeve koje dobije preko datavoda, a koji su neparni. U slucaju broja koji je paran stampa poruku "PARAN BROJ".*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_RANDOMS 10
#define MIN_VALUE 0
#define MAX_VALUE 99

int main()
{
    int pd[2];
    int pid;
    int random_nums[NUM_RANDOMS];
    int result;
    
    //Kreiranje datavoda
    if(pipe(pd) < 0)
    {
        printf("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }
    
    pid = fork(); //Kreiranje procesa deteta
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    if(pid == 0)
    {
        //Deo koda koji izvrsava proces dete
        close(pd[1]);
        
        int num;
        //Proces dete uzima od roditelja brojeve koje je on generisao
        //i ako su neparni stampa ih, a ako su parni stampa PARAN BROj
        while(read(pd[0], &num, sizeof(int)) > 0)
        {
            if(num %2 != 0)
            {
                printf("Proces dete od roditelja dobio: %d\n", num);
            }
            else
            {
                printf("PARAN BROJ!\n");
            }
        }
        
        close(pd[0]);
        exit(0);
    }
    else
    {
        //Deo koda koji izvrsava proces roditelj
        close(pd[0]);
        
        srand(time(NULL)); //Inicijalizacija generatora random brojeva
        
        //Proces roditelj generise 10 slucajnih brojeva od 0 do 99
        for(int i=0; i < NUM_RANDOMS; i++)
        {
            random_nums[i] = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
            write(pd[1], &random_nums[i], sizeof(int));
        }
        
        close(pd[1]);
        wait(NULL);
    }
    
    return 0;
}
