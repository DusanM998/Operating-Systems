/*Linux program koji simulira singronizacioni problem poznat pod nazivom "Vecera filozofa"*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMAFORI_KEY 10001
#define MUTEX_KEY 10002
#define STATUS_KEY 10005

//broj filozofa
#define N 5

//Enum koji definise moguce vrednosti za status u kome se filozof nalazi
union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
    struct seminfo* __buf;
    void * __pad;
};
