/*Septembar 2018*/
/*Linux program koji kreira dodatni proces sa kojim komunicira koriscenjem datavoda. Roditeljski proces generise 20 slucajnih brojeva u opsegu od 100 do 199 i salje ih putem datavoda procesu detetu. Proces dete na ekranu treba da odstampa samo brojeve koje dobije preko datavoda, a koji su deljivi sa 3. U slucaju broja koji nije deljiv sa 3 ne stampa nista*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int pid;
    int datavod[2];
    int i=0;
    int vrednost;
    
    if(pipe(datavod) < 0)
    {
        printf("Neuspesno otvaranje datavoda!\n");
        exit(-1);
    }
    
    pid = fork();
    if(pid == 0)
    {
        close(datavod[1]);
        while(read(datavod[0], &vrednost, sizeof(int)) > 0)
        {
            if(vrednost % 3 == 0)
                printf("Dete: %d\n", vrednost);
        }
        close(datavod[0]);
        exit(0);
    }
    else
    {
        close(datavod[0]);
        for(i=0; i < 20; i++)
        {
            vrednost = (rand()%100)+100;
            printf("Saljem: %d\n", vrednost);
            write(datavod[1], &vrednost, sizeof(int));
        }
        close(datavod[1]);
        wait(NULL);
    }
}
