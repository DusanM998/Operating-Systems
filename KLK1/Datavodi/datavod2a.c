#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int main(int argc, char* argv[])
{
    int deskriptori[2];
    char prenos[200];
    char prijem[200];
    int pid;
    
    if(pipe(deskriptori)<0)
    {
        printf("Doslo je do greske prilikom kreiranja PIPE-a\n");
        exit(-1);
    }
    
    pid = fork();
    if(pid == 0)
    {
        close(deskriptori[0]);
        sprintf(prenos, "%d", deskriptori[1]);
        execl("datavod2a","datavod2b", prenos, NULL);
        printf("Neuspesno!\n");
    }
    else
    {
        close(deskriptori[1]);
        read(deskriptori[0], prijem, 200*sizeof(char));
        printf("Ja sam prvi proces i primam: %s\n", prijem);
        exit(0);
    }
}
    
