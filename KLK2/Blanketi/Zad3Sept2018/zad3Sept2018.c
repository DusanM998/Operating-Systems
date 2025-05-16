/*Septembar2018 i Septembar 2020*/
/*Kreirati 2 Linux procesa koji komuniciraju koriscenjem mehanizma redova poruka(message queues). Prvi proces cita stringove sa tastature i upisuje ih u red poruka sve dok se na tastaturi ne unese rec KRAJ. Drugi proces cita stringove iz reda poruka(dok ne detektuje string KRAJ) i prikazuje ih na std izlazu. U sistemu postoji i treci proces koji kreira red poruka i pokrece ova 2 procesa (koriscenjem execl/execv sistemskog poziva), a zatim ceka da se oni zavrse i tek nakon toga brise red poruka i zavrsava svoje izvrsavanje.*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define MSG_KEY 10101

struct msgbuf
{
    long m_type;
    char m_text[300];
};

int main(int argc, char* argv[])
{
    int pid;
    int msg = msgget(MSG_KEY, 0666 | IPC_CREAT);
    struct msgbuf poruka;
    poruka.m_type = 1;
    
    pid = fork();
    if(pid == 0)
    {
        execl("zad3Sept2018b", "zad3Sept2018b", NULL);
        printf("Neuspesno pokretanje citaoca!\n");
        exit(-1);
    }
    
    pid = fork();
    if(pid == 0)
    {
        execl("zad3Sept2018a", "zad3Sept2018a", NULL);
        printf("Neuspesno pokretanje kreatora!\n");
    }
    
    wait(NULL);
    wait(NULL);
}
