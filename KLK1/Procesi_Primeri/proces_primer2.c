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
        sleep(4);
        printf("Ja sam proces dete, sa id-em: %d i upravo cu zameniti svoj kod!\n", getpid());
        execlp("ls", "ls", "-al", "/home/dusan98/Desktop/Operativni/Procesi_Primeri/",NULL);
        
        printf("Error, nisam uspeo!");

    }
    else
    {
        printf("Ja sam proces roditelj, sa id-em: %d.\n",getpid());   
    }
    
    printf("Ja sam proces roditelj i upravo zavrsavam:%d! Moje dete nece ovo videti jer je promenilo kod!\n",getpid());
    return 0;
}
    
