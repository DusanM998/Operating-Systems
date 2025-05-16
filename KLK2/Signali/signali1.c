#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

void do_nothing()
{
    printf("Novi alarm je zavrsen!\n");
    return;
}

int main(int argc, char* argv[])
{
    signal(SIGALRM, do_nothing);
    
    alarm(5);
    
    pause();
    
    signal(SIGALRM, SIG_DFL);
    
}
