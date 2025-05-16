/*Linux program koji simulira problem proizvodjac/potrosac koriscenjem redova poruka. Glavni program se deli u dva
 * procesa. Prvi proces(proizvodjac) kreira N slucajnih pozitivnih celih brojeva i salje ih drugom procesu. N se 
 * odredjuje tokom izvrsenja programa, takodje kao slucajan pozitivan ceo broj. Po zavrsetku slanja, prvi proces
 * salje -1 kao kod za kraj. Drugi proces(potrosac) preuzima poslate brojeve iz poruka i stampa ih na standardnom
 * izlazu. Nakon sto primi broj -1 proces stampa koliko je dobio parnih a koliko neparnih brojeva.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGLEN 50
#define RED_PORUKA 10104
#define MAX_PORUKA 100

struct mymsgbuf 
{
    long mtype;
    char mtext[MSGLEN];
};

int main()
{
    int pid;
    int msqid;
    int broj = 0;
    int brojParnih = 0;
    int brojNeparnih = 0;
    
    struct mymsgbuf bafer;
    
    pid = fork();
    
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    if(pid == 0)
    {
        //Proces dete
        if(execl("April2022a", "April2022a", NULL) < 0)
        {
            printf("Doslo je do greske prilikom ucitavanja izvrsne datoteke!\n");
            exit(1);
        }
    }
    
    //Kod koji izvrsava proces roditelj
    msqid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    //Primaju se poruke iz reda poruka sve dok ne stigne -1 poruka
    do
    {
        if(msgrcv(msqid, &bafer, MAX_PORUKA, 0, 0) < -1)
        {
            printf("Doslo je do greske prilikom prijema poruke!\n");
            exit(1);
        }
        broj = atoi(bafer.mtext);
        printf("Proces roditelj: Primio sam broj: %d\n", broj);
        
        if(broj % 2 == 0)
            brojParnih++;
        else
            brojNeparnih++;
    }
    while(broj != -1);
    
    printf("Broj parnih brojeva: %d\n", brojParnih);
    printf("Broj neparnih brojeva: %d\n", brojNeparnih);
    
    wait(NULL); //Ceka da se zavrsi proces dete
    
    msgctl(msqid, IPC_RMID, 0);
    
    return 0;
}
