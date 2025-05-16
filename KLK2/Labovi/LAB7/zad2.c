/*Tri Linux procesa koji za medjusobnu komunikaciju koriste deljenu memoriju velicine 1024 bajtova. Prvi proces popunjava prvih 512 bajtova deljive memorije slucajno izabranim slovima (u opsegu a-z). Nakon toga, drugi proces popunjava poslednjih 512 bajtova deljene memorije, proizvoljno izabranim ciframa. Posto i drugi proces zavrsi generisanje podataka, treci proces kompletan sadrzaj deljene memorije upisuje u datoteku. Ova sekvenca akcija se periodicno ponavlja svakih 15s.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define PRVI_KEY 10001
#define DRUGI_KEY 10002
#define TRECI_KEY 10003
#define MEM_KEY 10004
#define N 1024

union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
    struct seminfo* __buf;
    void* __pad;
};

int main()
{
    int sem1, sem2, sem3, memid; //Reference semafora i deljive memorije
    int i, j;
    FILE* f;
    
    //Pokazivac na niz bajtova koji se koristi za mapiranje deljive memorije
    char* shmem;
    
    //Pomocne strukture za inicijalizaciju i operacije nad semaforom
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    union semun opts;
    
    //Kreiramo i inicijaluzijemo semafore
    /*Inicijalno se postavljaju ne vrednosti 1-0-0 jer zapocinje da se izvrsava proces P1 pa tek onda P3*/
    sem1 = semget(PRVI_KEY, 1, IPC_CREAT | 0666);
    sem2 = semget(DRUGI_KEY, 1, IPC_CREAT | 0666);
    sem3 = semget(TRECI_KEY, 1, IPC_CREAT | 0666);
    opts.val = 1;
    semctl(sem1, 0, SETVAL, opts);
    opts.val = 0;
    semctl(sem2, 0, SETVAL, opts);
    opts.val = 0;
    semctl(sem3, 0, SETVAL, opts);
    
    //Kreiramo deljenu memoriju
    memid = shmget(MEM_KEY, N * sizeof(char), IPC_CREAT | 0666);
    if(fork() == 0)
    {
        //Proces P1 koji generise slova a-z i smesta ih u prvih 512 bajtova
        //Mapiramo deljenu memoriju
        shmem = (char*)shmat(memid, NULL, 0);
        for(i=0; i < 10; i++)
        {
            semop(sem1, &lock, 1); //P(S1)
            //Upisujemo slova u prvih 512 bajtova
            for(j=0; j < N/2; j++)
                shmem[i] = 'a' + rand() % ('z'-'a' + 1);
            semop(sem2, &unlock, 1); //V(S2)
        }
        
        //Izbacujemo deljenu memoriju iz ardesnog prostora
        shmdt(shmem);
        return 0;
    }
    
    if(fork() == 0)
    {
        //Proces P2 koji generise cifre i smesta ih u poslednjih 512 bajtova
        //Mapiramo deljenu memoriju
        shmem = (char*)shmat(memid, NULL, 0);
        for(i=0; i < 10; i++)
        {
            semop(sem2, &lock, 1); //P(S2)
            //Upisujemo slova u prvih 512 bajtova
            for(j=0; j < N/2; j++)
                shmem[j] = '0' + rand() % ('9'-'0' + 1);
            semop(sem3, &unlock, 1); //V(S3)
        }
        //Izbacujemo deljenu memoriju iz adresnog prostora
        shmdt(shmem);
        return 0;
    }
    
    //Roditeljski proces P3 koji stampa sadrzaj deljene memorije u datoteku
    //Mapiramo deljenu memoriju
    shmem = (char*)shmat(memid, NULL, SHM_RDONLY);
    for(i=0; i < 10; i++)
    {
        semop(sem3, &lock, 1); //P(S3)
        //sadrzaj deljene memorije upisujemo u datoteku
        char buff[N+1];
        /*Koristimo f-ju strcnpy jer u deljenoj memoriji na kraju nemamo /0 kao kraj stringa*/
        strncpy(buff, shmem, N);
        buff[N] = '\0';
        
        //Sadrzaj bafera upisujemo u datoteku
        f = fopen("sadrzaj", "a");
        fprintf(f, "%s\n", buff);
        fclose(f);
        semop(sem1, &unlock, 1); //V(S1)
    }
    
    //Izbacujemo deljenu meoriju iz adresnog prostora
    shmdt(shmem);
    semctl(sem1, 0, IPC_RMID, opts);
    semctl(sem2, 0, IPC_RMID, opts);
    semctl(sem3, 0, IPC_RMID, opts);
    semctl(memid, IPC_RMID, 0);
    
    return 0;
}
