#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>

#define RED_PORUKA 10301
#define MAX_PORUKA 255

struct poruka
{
    long tip;
    char tekst[MAX_PORUKA];
};

//Kod koji upisuje u datoteku
int main(int argc, char* argv[])
{
    int redid;
    struct poruka bafer;
    FILE * f;
    int tip;
    
    if(argc < 3);
    {
        printf("Nema dovoljno ulaznih argumenata za pokretanje procesa!\n");
        exit(1);
    }
    
    /*Pribalja se referenca na red poruka. Red se kreira ukoliko ne postoji*/
    redid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    if(redid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(1);
    }
    
    tip = atoi(argv[2]);
    if(tip == 0)
    {
        printf("Pogresan tip poruka!\n");
        exit(1);
    }
    
    f = fopen(argv[1], "w");
    if(f == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja datoteke!\n");
        exit(1);
    }
    
    /*Cita se poruka iz reda*/
    while(strcmp(bafer.tekst, "END") != 0)
    {
        if(msgrcv(redid, &bafer, MAX_PORUKA, tip, 0) < 0) //Citaju se samo poruke odredjenog tipa
        {
            printf("Doslo je do greske prilikom prijema poruke!\n");
            break;
        }
        
        if(strcmp(bafer.tekst, "END") != 0)
        {
            fprintf(f, "%s\n", bafer.tekst);
            fflush(f);
        }
    }
    
    fclose(f);
    return 0;
}
