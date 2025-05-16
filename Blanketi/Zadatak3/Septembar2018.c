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

int main()
{
    int msqid;
    
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    //Treci proces: kreira red poruka i pokrece prethodna 2 procesa
    //Kreiranje prvog procesa koji unosi stringove
    if(fork() == 0)
    {
        if(execl("Septembar2018a", "Septembar2018a", NULL) < 0)
        {
            printf("Doslo je do greske prilikom ucitavanja izvrsne datoteke\n");
            exit(1);
        }
    }
    
    //Kreiranje drugog procesa koji prikazuje stringove
    if(fork() == 0)
    {
        if(execl("Septembar2018b", "Septembar2018b", NULL) < 0)
        {
            printf("Doslo je do greske prilikom ucitavanja izvrsne datoteke\n");
            exit(1);
        }
    }
    
    //Cekanje da se oba procesa zavrse
    wait(NULL);
    wait(NULL);
    
    if(msgctl(msqid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }
    
    printf("Svi procesi zavrseni. Red poruka obrisan.\n");
    
    return 0;
}
