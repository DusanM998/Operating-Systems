/*Linux program koji kreira dodatni proces dete. Ova dva procesa komuniciraju koriscenjem jenog reda poruka. 
 * Prvi proces sa tastature cita cele visecifrene brojeve i salje ih drugom procesu koristeci red poruka.
 * Drugi proces treba da sabere cifre primljenog broja i da ih odstampa na standardnom izlazu. Ovaj postupak se ponavlja 10 puta.*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGLEN 50
#define RED_PORUKA 10104
#define NUM_ITERATIONS 10

struct mymsgbuf 
{
    long mtype;
    char mtext[MSGLEN];
};

int main()
{
    int msqid;
    struct mymsgbuf buf;
    
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    if(fork() == 0) //Proces dete
    {
        for(int i=0; i < NUM_ITERATIONS; i++)
        {
            if(msgrcv(msqid, &buf, MSGLEN, 1, 0) == -1)
            {
                printf("Doslo je do greske prilikom prijema poruke!\n");
                exit(1);
            }
            
            int broj = atoi(buf.mtext);
            if(broj == 0)
                break;
            
            int zbir = 0;
            int temp = broj;
            while(temp > 0)
            {
                zbir += temp % 10;
                temp /= 10;
            }
            
            printf("Proces dete: Primio sam broj: %d\n", broj);
            printf("Proces dete: Zbir cifara broja: %d: %d\n", broj, zbir);
        }
        exit(0);
    }
    else
    {
        int broj;
        for(int i=0; i < NUM_ITERATIONS; i++)
        {
            printf("Unesite broj: \n");
            scanf("%d", &broj);
            
            //Slanje samo visecifrenih brojeva
            if(broj > 10)
            {
                buf.mtype = 1;
                sprintf(buf.mtext, "%d", broj);
                msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);
            }
            else
            {
                printf("Broj nije visecifren, unesite visecifren broj!\n");
                continue;
            }
        }
        
        wait(NULL); //Ceka da se zavrsi proces dete
    
        msgctl(msqid, IPC_RMID, NULL); //Uklanjanje reda poruka
    }
    
    return 0;
}
