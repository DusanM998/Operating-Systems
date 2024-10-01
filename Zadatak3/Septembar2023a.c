/*Linux program koji ucitava podatke iz tekst. datoteke "poruka.txt" (red po red) i zatim koriscenjem jednog reda poruka
 * sve parne redove salje procesu koji konvertuje sva slova u velika, a sve neprne redove salje procesu koji konvertuje
 * sva slova u mala. Drugi i treci proces konvertovane linije upisuju u datoteku "sredjeno.txt".*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define RED_PORUKA 10301
#define MAX_PORUKA 255

struct poruka 
{
    long tip;
    char tekst[MAX_PORUKA];
};

int main(int argc, char *argv[])
{
    int redid;
    struct poruka bafer;
    FILE *f;
    
    redid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    if(redid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    tip = atoi(argv[1]);
    if(tip == 0)
    {
        printf("Pogresan tip poruka!\n");
        exit(1);
    }
    
    //Cita poruke iz reda
    while(strcmp(bafer.tekst, "END") != 0)
    {
        if(msgrcv(redid, &bafer, MAX_PORUKA, tip, 0) < 0)
        {
            printf("Doslo je do greske prilikom prijema poruke!\n");
            break;
        }
        if(strcmp(bafer.tekst, "END") != 0)
        {
            fprintf(f, "%s", bafer.tekst);
            fflush(stdout);
        }
    }
    
    return 0;
}
