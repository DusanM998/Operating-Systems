/*Koriscenjem prog. jez. C kreirati dva Linux procesa koji komuniciraju koriscenjem redova poruka. Prvi proces cita brojeve sa tastature
 * i upisuje ih u red poruka sve dok suma unetih brojeva ne postane veca od 1000. Drugi proces cita brojeve iz reda poruka (sve dok suma
 * dobijenih brojeva ne postane veca od 1000) i prikazuje ih na standardnom izlazu. U sistemu postoji i treci proces koji kreira red poruka
 * i pokrece ova dva procesa (koriscenjem execl/execv sistemskog poziva), a zatim ceka da se oni zavrse i tek nakon toga brise red poruka i izlazi.*/

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
    int suma = 0;
    int pid1;
    
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    //Drugi proces - cita brojeve iz reda poruka
    while(suma <= 1000)
    {
        msgrcv(msqid, &buf, MSGLEN, 1, 0);
        int broj = atoi(buf.mtext);
        suma += broj;
        printf("Primljen broj od 1. procesa: %d\n", broj);
        if(suma > 1000)
        {
            printf("Suma %d veca od 1000. Izlazim...\n", suma);
        }
    }
    
    return 0;
}
