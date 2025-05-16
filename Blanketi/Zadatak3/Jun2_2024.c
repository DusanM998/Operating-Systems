/*Linux program koji kreira dodatni proces dete. Proces roditelj sa tastature cita brojeve i salje procesu detetu koriscenjem reda poruka
 * samo trocifrene brojeve. Proces dete sabira cifre dobijenih brojeva i zbir cifara svakog broja upisuje u fajl. Komunikacija se vrsi
 * sve dok korisnik ne unese broj 0 nakon cega oba procesa izlaze.*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGLEN 50
#define RED_PORUKA 10104

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
        FILE *file = fopen("zbir_cifara.txt", "w");
        if(!file)
        {
            printf("Greska prilikom otvaranja fajla!\n");
            exit(1);
        }
        
        while(1)
        {
            msgrcv(msqid, &buf, MSGLEN, 1, 0);
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
            
            fprintf(file, "Broj: %d, Zbir cifara: %d\n", broj, zbir);
        }
        
        fclose(file);
        exit(0);
    }
    else //Proces roditelj
    {
        while(1)
        {
            int broj;
            printf("Unesite broj: (0 za kraj): \n");
            scanf("%d", &broj);
            
            if(broj < 100 && broj > 999)
            {
                printf("Unosite samo trocifrene brojeve!\n");
                continue;
            }
            
            //Ako se unese 0 salje se procesu detetu i signalizira se kraj komunikacije
            if(broj == 0)
            {
                buf.mtype = 1;
                sprintf(buf.mtext, "%d", broj);
                msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);
                break;
            }
            
            //Slanje samo trocifrenih brojeva
            if(broj >= 100 && broj <= 999)
            {
                buf.mtype = 1;
                sprintf(buf.mtext, "%d", broj);
                msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);
            }
        }
        
        wait(NULL); //Ceka da se zavrsi proces dete
    
        msgctl(msqid, IPC_RMID, NULL); //Uklanjanje reda poruka
    }
    
    return 0;
}
