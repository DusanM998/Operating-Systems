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

#define RED_PORUKA 10104

int main()
{
    //Treci proces: kreira red poruka i pokrece prethodna dva procesa
    int msqid;
    
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    //Kreiranje prvog procesa koji unosi brojeve
    if(fork() == 0)
    {
        if(execl("Septembar2024a", "Septembar2024a", NULL) < 0)
        {
            printf("Doslo je do greske prilikom ucitavanja izvrsne datoteke!\n");
            exit(1);
        }
    }
    
    //Kreiranje drugog procesa - prikazuje brojeve
    if(fork() == 0)
    {
        if(execl("Septembar2024b", "Septembar2024b", NULL) < 0)
        {
            printf("Doslo je do greske prilikom ucitavanja izvrsne datoteke!\n");
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
