/*Linux program koji kopira zadatu datoteku. Prvi argument komandne linije je naziv originalne datoteke, a drugi argument je naziv nove datoteke, koja ce biti kopija prve. Kopiranje se obavlja tako sto se program deli u 2 procesa, prvi proces cita odredjeni broj bajtova iz datoteke i salje ih drugom procesu. Drugi proces bajtove iz deljene memorije upisuje u novu datoteku*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MEM_KEY 1122
#define PROC_A_KEY 10101
#define PROC_B_KEY 10102
#define BUF_SIZE 80


union semun
{
    int val;
    struct semid_ds *buf;
    ushort* array;
};

struct rdstruct
{
    char nch;
    char buf[BUF_SIZE];
};

int main(int argc, char* argv[])
{
    int i, procaid, procbid;
    union semun semopts;
    struct sembuf sem_lock = {0, -1, 0};
    struct sembuf sem_unlock = {0, 1, 0};
    
    procaid = semget((key_t)PROC_A_KEY, 1, 0666 | IPC_CREAT);
    procbid = semget((key_t)PROC_B_KEY, 1, 0666 | IPC_CREAT);
    semopts.val = 1;
    semctl(procaid, 0, SETVAL, semopts);
    semopts.val = 0;
    semctl(procbid, 0, SETVAL, semopts);
    printf("argv[1]: %s, argv[2]: %s\n", argv[1], argv[2]);
    
    if(fork() != 0)
    {
        int shm_id = shmget(MEM_KEY, sizeof(struct rdstruct), IPC_CREAT | 0666);
        struct rdstruct *shm_ptr = shmat(shm_id, NULL, 0);
        
        int fd = open(argv[1], O_RDONLY);
        shm_ptr->nch = BUF_SIZE;
        
        while(shm_ptr->nch == BUF_SIZE)
        {
            semop(procaid, &sem_lock, 1);
            shm_ptr->nch = read(fd, shm_ptr->buf, BUF_SIZE);
            printf("Upisao sam u shm %d bajtova: %s\n", shm_ptr->nch, shm_ptr->buf);
            fflush(stdout);
            semop(procbid, &sem_unlock, 1);
        }
        close(fd);
        shmdt(shm_ptr);
        printf("Proces roditelj zavrsio, ceka dete!\n");
        fflush(stdout);
        
        int status;
        wait(&status);
        
        shmctl(shm_id, IPC_RMID, 0);
        semctl(procaid, IPC_RMID, 0);
        semctl(procbid, IPC_RMID, 0);
        close(fd);
        
        printf("Proces roditelj sacekao dete, izlazi!\n");
        fflush(stdout);
    }
    else
    {
        int shm_id = shmget(MEM_KEY, sizeof(struct rdstruct), IPC_CREAT | 0666);
        struct rdstruct *shm_ptr = shmat(shm_id, NULL, 0);
        
        int fd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 00777);
        int n = BUF_SIZE;
        
        while(n == BUF_SIZE)
        {
            semop(procbid, &sem_lock, 1);
            write(fd, shm_ptr->buf, shm_ptr->nch);
            n = shm_ptr->nch;
            printf("Procitao sam iz shm %d bajtova: %s\n", shm_ptr->nch, shm_ptr->buf);
            fflush(stdout);
            semop(procaid, &sem_unlock, 1);
        }
        close(fd);
        shmdt(shm_ptr);
        
        printf("Proces dete zavrsio!\n");
        fflush(stdout);
    }
}
