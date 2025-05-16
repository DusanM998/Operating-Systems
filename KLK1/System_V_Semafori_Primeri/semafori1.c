#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <semaphore.h>

int main()
{
    int sid;
    
    union semun semopts;
    semopts.val = 1;
    
    sid = semget((key_t)333,1,0666|IPC_CREAT);
    if(sid<0)
    {
        printf("Semafor nije uspesno kreiran!");
        exit(-1);
    }
    
    if((semctl(sid,0,SETVAL,semopts) == -1))
    {
        printf("Greska prilikom postavljanja vrednosti semafora!");
        exit(-1);
    }
    
    semctl(sid,0,IPC_RMID,0);
}
