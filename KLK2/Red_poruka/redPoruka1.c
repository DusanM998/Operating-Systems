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
    char mtext[100];
};

int main(int argc, char* argv[])
{
    int msgque;
    int msglen = 9;
    char msg[100];
    
    msgque = msgget(QUEUE_KEY, 0666|IPC_CREAT);
    if(msgque == -1)
    {
        printf("Neuspesno pribavljanje poruka!\n");
        exit(-1);
    }
    
    struct msgbuf poruka;
    
    if(msgrcv(msgque, &poruka, msglen, 0, 0) == -1)
        printf("Poruka nije primljena!\n");
    else
        strcpy(msg, poruka.mtext);
        printf("Poruka je uspesno primljena i glasi: %s\n",msg);
}
