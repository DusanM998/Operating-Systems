#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

//Radi i sa stringovima
int main(int argc, char* argv[])
{
    int deskriptori[2];
    char poruka[200];
    char primanje[200];
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
        read(deskriptori[0],primanje,200*sizeof(char));
        printf("Ja sam primaoc i primio sam: %s\n", primanje);
        close(deskriptori[0]);
    }
    else
    {
        close(deskriptori[0]);
        printf("Ja sam posiljaoc, unesite poruku koju zelite da posaljete: \n");
        fgets(poruka, 200, stdin);
        printf("\n");
        write(deskriptori[1], poruka, 200*sizeof(char));
        close(deskriptori[1]);
        wait(NULL);
    }
}
    
