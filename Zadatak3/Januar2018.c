/*Linux program koji kreira dodatni proces dete. Ova dva procesa komuniciraju dvosmerno koriscenjem jednog reda poruka.
 * Prvi proces sa tastature cita cele brojeve u vidu stringova i salje ih drugom procesu koristeci red poruka.
 * Drugi proces, tako primljen broj mnozi sa 2(duplira) i rezultat vraca prvom procesu. Prvi proces tako dobijene brojeve
 * stampa na ekranu. Ovaj postupak se ponavlja 10 puta.*/

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
    
    if(fork() == 0) //Proces dete: Primljene brojeve duplira i salje drugom procesu
    {
        for(int i=0; i < NUM_ITERATIONS; i++)
        {
            if(msgrcv(msqid, &buf, sizeof(buf.mtext), 1, 0) == -1)
            {
                printf("Greska prilikom prijema poruke!\n");
                exit(1);
            }
            int broj = atoi(buf.mtext);
            
            if(broj == 0)
            {
                // Ako je primljena poruka "0", zavrsi proces
                printf("Proces dete: Dobio sam 0. Ponovni unos.\n");
                continue;
            }
            
            broj *= 2;
            
            //Salje duplirane brojeve prvom procesu(roditelju)
            buf.mtype = 2;
            sprintf(buf.mtext, "%d", broj);
            if(msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0) == -1)
            {
                perror("Greska pri slanju rezultata roditelju!\n");
                exit(1);
            }
        }
        exit(0);
    }
    else
    {
        //Proces roditelj: cita cele brojeve u vidu stringova
        //i salje ih drugom procesu(detetu)
        int broj;
        for(int i=0; i < NUM_ITERATIONS; i++)
        {
            //Unosi brojeve u vidu stringova
            printf("Proces roditelj: Unesite broj: \n");
            fgets(buf.mtext, MSGLEN, stdin);
            buf.mtext[strcspn(buf.mtext, "\n")] = 0;
            
            //Salje broj prvom procesu(detetu)
            buf.mtype = 1;
            if(msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0) == -1)
            {
                printf("Doslo je do greske prilikom slanja poruke!\n");
                exit(1);
            }
            
            // Ako je poslat broj "0", prekini ciklus
            int broj = atoi(buf.mtext);
            if(broj == 0)
            {
                printf("Proces roditelj: Poslao sam 0. Ponovni unos.\n");
                continue;
            }
            
            //Prima duplirani broj od procesa deteta
            if(msgrcv(msqid, &buf, sizeof(buf.mtext), 2, 0) == -1)
            {
                printf("Doslo je do greske prilikom primanja rezultata od deteta!\n");
                exit(1);
            }
            
            //Ispis rezultata
            printf("Proces roditelj: Od deteta sam dobio duplirani broj: %s\n",buf.mtext); 
        }
        
        wait(NULL); //Ceka da se zavrsi proces dete
    
        msgctl(msqid, IPC_RMID, NULL); //Uklanjanje reda poruka
    }
    return 0;
}
