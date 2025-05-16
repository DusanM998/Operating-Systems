/*Linux program koji kreira dodatni proces dete. Ova 2 procesa komuniciraju dvosmerno koriscenjem jednog reda poruka. Prvi proces sa tastature cita cele brojeve u vidu stringova i salje ih drugom procesu koristeci red poruka. Drugi proces, tako primljen broj mnozi sa 2(duplira) i rezultat vraca prvom procesu. Prvi proces tako dobijeni broj stampa na ekranu. Ovaj postupak se ponavlja 10 puta*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_KEY 10101

struct msgbuf
{
    long m_type; //1->odlazna, 2->dolazna za proces roditelj
    char m_text[300];
};

int main(int argc, char* argv[])
{
    int msg = msgget(MSG_KEY, IPC_CREAT | 0666);
    struct msgbuf poruka;
    int counter = 0;
    int pid;
    
    pid = fork();
    if(pid == 0)
    {
        while(counter < 10)
        {
            poruka.m_type = 1;
            
            int broj;
            msgrcv(msg, &poruka, sizeof(struct msgbuf), 1, 0);
            
            broj = atoi(poruka.m_text);
            broj*=2;
            sprintf(poruka.m_text, "%d", broj);
            
            poruka.m_type = 2;
            msgsnd(msg, &poruka, sizeof(struct msgbuf), 0);
            
            counter++;
        }
    }
    else
    {
        while(counter < 10)
        {
            fgets(poruka.m_text, 300, stdin);
            poruka.m_text[strlen(poruka.m_text)-1] = '\0';
            
            poruka.m_type = 1;
            
            msgsnd(msg, &poruka, sizeof(struct msgbuf), 0);
            
            poruka.m_type = 2;
            msgrcv(msg, &poruka, sizeof(struct msgbuf), 2, 0);
            
            printf("Roditelj primio sam: %s\n", poruka.m_text);
            
            counter++;
        }
        wait(NULL);
        msgctl(msg, IPC_RMID, NULL);
        exit(0);
    }
}
