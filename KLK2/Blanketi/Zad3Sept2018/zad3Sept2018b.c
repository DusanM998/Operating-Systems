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
    int msg = msgget(MSG_KEY, 0666);
    struct msgbuf poruka;
    poruka.m_type = 1;
    
    msgrcv(msg, &poruka, sizeof(struct msgbuf), 0, 0);
    while(strcmp(poruka.m_text, "KRAJ") != 0)
    {
        printf("Dete primilo: %s\n", poruka.m_text);
        msgrcv(msg, &poruka, sizeof(struct msgbuf), 0, 0);
    }
    exit(0);
}
