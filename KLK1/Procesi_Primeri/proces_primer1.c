#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    int pid; //id procesa
    pid = fork();
    if(pid == 0)
    {
        sleep(5);
        printf("Ja sam proces dete, sa id-em: %d, a moj roditelj je: %d\n",getpid(),getppid());
    }
    else
    {
        printf("Ja sam proces roditelj, sa id-em: %d, a moje dete je: %d\n",getpid(),pid);
        waitpid(pid, NULL, 0);
    }
    
    printf("Ja se upravo zavrsavam:%d!\n",getpid());
    return 0;
}
    
