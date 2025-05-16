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
    int N; //Broj poruka koje ce biti poslate
    int msqid;
    struct mymsgbuf bafer;
    
    msqid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    N = rand() % 100; //Odredjuje se broj poruka koje ce biti poslate
    
    for(int i=0; i < N + 1; i++)
    {
        if(i == N)
            sprintf(bafer.mtext, "%d", -1); //Poslednja poruka je -1
        else
            sprintf(bafer.mtext, "%d", rand() % 100);
        
        bafer.mtype = 1;
        
        //Poruke se salju u red
        if(msgsnd(msqid, &bafer, sizeof(bafer.mtext), 0) < -1)
        {
            printf("Doslo je do greske prilikom prijema poruke!\n");
            exit(1);
        }
    }
    
    return 0;
}
