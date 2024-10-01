/* Koriscenjem prog. jez. C kreirati 2 Linux procesa koji komuniciraju koriscenjem meganizma redova poruka(message queues).
 * Prvi proces cita stringove sa tastature i upisuje ih u red poruka sve dok se na tastaturi ne unese rec KRAJ. Drugi proces
 * cita stringove iz reda poruka (dok ne detektuje string KRAJ) i prikazuje ih na standardnom izlazu. U sistemu postoji i treci
 * proces koji kreira red poruka i pokrece ova dva procesa(koriscenjem execl/execv sistemskih poziva), a zatim ceka da se oni zavrse
 * i tek nakon toga brise red poruka i zavrsava svoje izvrsavanje*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGLEN 50
#define RED_PORUKA 10301

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
    
    //Drugi proces: cita stringove iz reda poruka(dok ne detektuje KRAJ)
    do
    {
        if(msgrcv(msqid, &buf, MSGLEN, 1, 0) == -1)
        {
            printf("Doslo je do greske prilikom prijema poruke!\n");
            exit(1);
        }
        
        
        printf("2. Proces: Primio sam od 1. procesa string: %s\n", buf.mtext);
        
    }
    while(strcmp(buf.mtext, "KRAJ") != 0);
    
    return 0;
}
