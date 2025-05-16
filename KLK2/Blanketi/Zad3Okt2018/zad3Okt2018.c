/*Linux program koji omogucava da 2 procesa komuniciraju i sinhronizuju svoje izvrsavanje koriscenjem signala i deljive memorije. Prvi proces po slucajnom principu kreira 1024 celobrojne vrednosti i smesta ih u deljeni memorijski prostor. Kada izgenerise brojeve, obavestava o tome drugi proces, slanjem signala SIGUSR1 i pauzira svoje izvrsavanje. Kada drugi proces primi signal, on sadrzaj deljenog memorijskog prostora prikazuje na std izlaz, salje prvom procesu signal SIGUSR2 i nakon toga pauzira svoje izvrsavanje. Po prijemu signala SIGURS2, prvi proces ponovo startuje citav postupak. Postupak se ponavlja 4096 puta. Nakon toga se oba procesa zavrsavaju.*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>

#define SHM_KEY 10101

void catchSigUser1()
{
    return;
}

void catchSigUser2()
{
    return;
}

int main()
{
    int shm = shmget(SHM_KEY, 1024 * sizeof(int), 0666 | IPC_CREAT);
    int* memoryPointer;
    int pid;
    signal(SIGUSR1, catchSigUser1);
    signal(SIGUSR2, catchSigUser2);
    pid = fork();
    if(pid == 0)
    {
        memoryPointer = shmat(shm, NULL, 0);
        int j=0;
        while(j < 2)
        {
            pause();
            for(int i=0; i < 1024; i++)
                printf("%d\n", memoryPointer[i]);
            fflush(stdout);
            int rod = getppid();
            
            kill(rod, SIGUSR2);
            j++;
        }
        shmdt(memoryPointer);
        exit(0);
    }
    else
    {
        int j = 0;
        memoryPointer = shmat(shm, NULL, 0);
        while(j < 2)
        {
            for(int i=0; i < 1024; i++)
                memoryPointer[i] = rand()%100;
            sleep(1);
            kill(pid, SIGUSR1);
            pause();
            j++;
            sleep(5);
        }
        wait(NULL);
        shmdt(memoryPointer);
        shmctl(shm, IPC_RMID, NULL);
    }
}
