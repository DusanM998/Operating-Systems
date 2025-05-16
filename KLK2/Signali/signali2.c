#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int pid;
void kraj();

int main(int argc, char* argv[])
{
    signal(SIGALRM, kraj);
    pid = fork();
    if(pid == 0)
    {
        execl("charcount", "charcount", NULL);
        printf("Neuspesno otvaranje charcount-a!\n");
        exit(0);
    }
    else
    {
        alarm(1);
        pause();
    }
    
}

void kraj()
{
    printf("Isteklo je vreme za brojanje karaktera!\n");
    kill(pid, SIGKILL);
}
