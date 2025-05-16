/*Linux program koji kreira dodatni proces dete. Proces roditelj iz datoteke brojevi.txt cita niz od 10 celobrojnih vrednosti i upisuje ih u deljenu memoriju
 * Proces dete cita 10 brojeva iz deljene memorije i na ekranu stampa njihov zbir. Postupak se ponavlja sve dok proces roditelj ne procita sve brojeve 
 * iz datoteke brojevi.txt. Ukoliko broj brojeva u datoteci nije deljiv sa 10, proces roditelj u poslednjoj iteraciji dopunjuje nulama procitani niz brojeva.
 * Za sinhronizaciju procesa koristiti System V semafore.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define MEM_KEY 1122
#define PROC_A_KEY 10101
#define PROC_B_KEY 10102
#define NUM_COUNT 10

union semun 
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

void dopuni_nulama(int* numbers, int read_count) 
{
    for (int i = read_count; i < NUM_COUNT; i++) 
    {
        numbers[i] = 0; // Dopunjujemo nulama
    }
}

int main()
{
    int procaid, procbid;
    int brojevi[10] = {0};
    union semun semopts;
    struct sembuf sem_lock = {0, -1, 0};
    struct sembuf sem_unlock = {0, 1, 0};
    
    procaid = semget((key_t)PROC_A_KEY, 1, 0666 | IPC_CREAT);
    procbid = semget((key_t)PROC_B_KEY, 1, 0666 | IPC_CREAT);

    semopts.val = 1;
    semctl(procaid, 0, SETVAL, semopts);
    
    semopts.val = 0;
    semctl(procbid, 0, SETVAL, semopts);
    
    if(fork() != 0)
    {
        FILE *f = fopen("brojevi.txt", "r");
        if (f == NULL) {
            perror("Greska prilikom otvaranja datoteke");
            exit(1);
        }

        int shm_id = shmget(MEM_KEY, NUM_COUNT * sizeof(int), IPC_CREAT | 0666);  // Deljena memorija za 10 brojeva
        int* shm_ptr = shmat(shm_id, NULL, 0);  // Priključivanje na deljenu memoriju

        int numbers[NUM_COUNT];
        int read_count;
        
        while (1) {
            // Citanje brojeva iz datoteke
            semop(procaid, &sem_lock, 1);  // Zaključavanje roditeljskog semafora

            read_count = 0;
            for (int i = 0; i < NUM_COUNT && fscanf(f, "%d", &numbers[i]) == 1; i++) {
                read_count++;
            }

            // Ako nema više brojeva za čitanje, završavamo
            if (read_count == 0) 
            {
                semop(procbid, &sem_unlock, 1);  // Otključavanje semafora za dete (poslednja iteracija)
                break;
            }
            // Dopunjujemo nulama ako je broj pročitanih brojeva manji od 10
            if (read_count < NUM_COUNT) 
            {
                dopuni_nulama(numbers, read_count);
            }
            
            // Kopiranje brojeva u deljenu memoriju
            memcpy(shm_ptr, numbers, NUM_COUNT * sizeof(int));
            
            semop(procbid, &sem_unlock, 1);  // Otključavanje semafora za dete
        }
        
        fclose(f);
        shmdt(shm_ptr); 
        int status;
        wait(&status);  // Čekanje da dete završi
        shmctl(shm_id, IPC_RMID, NULL);  // Brisanje deljene memorije
        semctl(procaid, 0, IPC_RMID, NULL);  // Brisanje semafora
        semctl(procbid, 0, IPC_RMID, NULL);
        
    } 
    else 
    {  // Proces dete
        int shm_id = shmget(MEM_KEY, NUM_COUNT * sizeof(int), IPC_CREAT | 0666);  // Deljena memorija za 10 brojeva
        int* shm_ptr = shmat(shm_id, NULL, 0);  // Priključivanje na deljenu memoriju

        int numbers[NUM_COUNT];
        int sum;

        while (1) 
        {
            // Čitanje brojeva iz deljene memorije
            semop(procbid, &sem_lock, 1);  // Čekanje na roditeljski unos

            // Kopiramo brojeve iz deljene memorije
            memcpy(numbers, shm_ptr, NUM_COUNT * sizeof(int));

            // Ako su svi brojevi 0, prekidamo
            sum = 0;
            for (int i = 0; i < NUM_COUNT; i++) 
            {
                sum += numbers[i];
            }

            if (sum == 0) 
            {
                break;  // Zbir je 0, prekidamo
            }

            // Ispisivanje zbira
            printf("Zbir procitanih brojeva je: %d\n", sum);

            semop(procaid, &sem_unlock, 1);  // Otključavanje roditeljskog semafora za novu iteraciju
        }

        shmdt(shm_ptr); 
    }

    return 0;
}
