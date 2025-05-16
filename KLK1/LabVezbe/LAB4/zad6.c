/*Linux program koji se deli u 2 procesa. Jedan proces cita liniju po liniju dattoteku "prva.txt", a drugi datoteku "druga.txt". Ova 2 procesa upisuju naizmenicno procitane linije u txt datoteku "zbir.txt" tako da su sve neparne linije iz datoteke "prva.txt", a parne iz datoteke "druga.txt"*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define MUTEX_KEY 10101
#define PROC_A_KEY 10102
#define PROC_B_KEY 10103

#define DUZINA 80

//IPC System V semafori koji se koriste za sinhronizaciju procesa
union semun //mora uvek biti explicitno definisana
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int main()
{
    int mutexID, procaID, procbID, retCode;
    union semun semopts;
    char linija[DUZINA];
    FILE* fileA;
    FILE* fileB;
    FILE* fileC;
    
    struct sembuf sem_lock = {0, -1, 0}; //dekrementiranje semafora
    struct sembuf sem_unlock = {0, 1, 0}; //inkrementiranje semafora
    
    /*Kreiranje semafora*/
    mutexID = semget((key_t)MUTEX_KEY, 1, 0666 | IPC_CREAT);
    procaID = semget((key_t)PROC_A_KEY, 1, 0666 | IPC_CREAT);
    procbID = semget((key_t)PROC_B_KEY, 1, 0666 | IPC_CREAT);
    
    /*Inicijalizacija semafora*/
    semopts.val = 1;
    semctl(mutexID, 0, SETVAL, semopts);
    semopts.val = 1;
    semctl(procaID, 0, SETVAL, semopts);
    semopts.val = 0;
    semctl(procbID, 0, SETVAL, semopts);
    
    if(fork() == 0)
    {
        mutexID = semget((key_t)MUTEX_KEY, 1, 0666);
        procaID = semget((key_t)PROC_A_KEY, 1, 0666);
        procbID = semget((key_t)PROC_B_KEY, 1, 0666);
        fileA = fopen("prva.txt", "r");
        
        while(!feof(fileA))
        {
            fgets(linija, DUZINA, fileA);
            semop(procaID,&sem_lock, 1);
            semop(mutexID, &sem_lock, 1);
            
            fileC = fopen("zbir.txt", "a");
            
            fprintf(fileC, "%s", linija);
            fclose(fileC);
            semop(mutexID, &sem_unlock, 1);
            semop(procbID, &sem_unlock, 1);
        }
        
        fclose(fileA);
        exit(0);
    }
    else
    {
        mutexID = semget((key_t)MUTEX_KEY, 1, 0666);
        procaID = semget((key_t)PROC_A_KEY, 1, 0666);
        procbID = semget((key_t)PROC_B_KEY, 1, 0666);
        fileB = fopen("druga.txt", "r");
        
        while(!feof(fileB))
        {
            fgets(linija, DUZINA, fileB);
            semop(procbID, &sem_lock, 1);
            semop(mutexID, &sem_lock, 1);
            fileC = fopen("zbir.txt", "a");
            fprintf(fileC, "%s", linija);
            fclose(fileC);
            semop(mutexID, &sem_unlock, 1);
            semop(procaID, &sem_unlock, 1);
        }
        
        fclose(fileB);
        
        wait(&retCode);
        semctl(mutexID, 0, IPC_RMID, 0);
        semctl(procaID, 0, IPC_RMID, 0);
        semctl(procbID, 0, IPC_RMID, 0);
    }
}
