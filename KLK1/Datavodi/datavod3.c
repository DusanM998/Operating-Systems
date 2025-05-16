#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//Radi samo sa celobrojnim vrednostima(int)
int main (int argc, char* argv[])
{
    int deskriptori[2];
    int poruka;
    int primanje;
    int pid;
    
    if(pipe(deskriptori)<0)
    {
        printf("Doslo je do greske prilikom kreiranja PIPE-a\n");
        exit(-1);
    }
    
    pid = fork();
    if(pid == 0)
    {
        close(deskriptori[1]);
        read(deskriptori[0], &primanje, sizeof(int));
        printf("Ja sa primaoc i primio sam: %d\n", primanje);
        close(deskriptori[0]);
    }
    else
    {
        close(deskriptori[0]);
        printf("Ja sam posiljaoc, unesite poruku koju zelite da posaljete: \n");
        scanf("%d", &poruka);
        printf("\n");
        write(deskriptori[1], &poruka, sizeof(int));
        close(deskriptori[1]);
        wait(NULL);
    }
}
