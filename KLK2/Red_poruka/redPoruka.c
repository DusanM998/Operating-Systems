#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 10201

struct msgbuf
{
    long mtype;
    char mtext[7];
};

int main(int arg, char* argv)
{
    int msgque;
    char msg[] = "Poruka";
    
    msgque = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if(msgque < 0)
    {
        printf("Neuspesno pribavljanje reda poruka!\n");
        exit(-1);
    }
    
    struct msgbuf poruka;
    poruka.mtype = 1;
    strcpy(poruka.mtext, msg);
    
    if(msgsnd(msgque, &poruka, 7, 0) == -1)
        printf("Neuspesno slanje!\n");
    else
        printf("Poruka poslata uspesno!\n");
}
