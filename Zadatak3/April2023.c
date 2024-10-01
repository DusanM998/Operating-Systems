/*Koriscenjem prog. jez. C kreirati dva Linux procesa (proces roditelj i dete) koji komuniciraju koriscenjem mehanizma
 * redova poruka. Prvi proces cita brojeve sa tastature i upisuje ih u red poruka sve dok se na tastaturi ne unese broj 0.
 * Brojevi koji se unose su u intervalu [0 - 127]. Drugi proces cita brojeve iz reda poruka i na standardnom izlazu
 * ispisuje ASCII karakter koji odgovara dobijenom broju. Nakon sto drugi proces dobije iz reda poruka broj 0, ispisuje koliko je
 * ukupno brojeva primljeno preko reda poruka i izlazi.*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGLEN 50
#define RED_PORUKA 10401

struct mymsgbuf 
{
    long mtype;
    char mtext[MSGLEN];
};

int main()
{
    int msqid;
    struct mymsgbuf buf;
    int broj;
    
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    if(fork() == 0)
    {
        int primljenoBrojeva = 0;
        while(1)
        {
            if(msgrcv(msqid, &buf, MSGLEN, 1, 0) == -1)
            {
                printf("Doslo je do greske prilikom prijema poruke!\n");
                exit(1);
            }
            
            broj = atoi(buf.mtext);
            if(broj == 0)
                break;
            
            printf("ASCII karakter koji odgovara broju %d je %c\n", broj, (char)broj);
            primljenoBrojeva++;
        }
        printf("Primljeno ukupno %d brojeva.\n", primljenoBrojeva);
        msgctl(msqid, IPC_RMID, NULL); //Brisanje redova poruka
        exit(0);
    }
    else //Proces roditelj
    {
        buf.mtype = 1;
        while(1)
        {
            printf("Unesite broj (0 za kraj): \n");
            scanf("%d", &broj);
            if (broj < 0 || broj > 127) 
            {
                printf("Broj mora biti u intervalu [0 - 127]. Pokusajte ponovo.\n");
                continue;
            }
            sprintf(buf.mtext, "%d", broj);
            if(msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }
            
            if(broj == 0)
                break;
            
        }
        
        wait(NULL); //Ceka da se zavrsi proces dete
    
        msgctl(msqid, IPC_RMID, NULL); //Uklanjanje reda poruka
    }
    
    return 0;
}
