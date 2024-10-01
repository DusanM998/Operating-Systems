/*Linux program koji kreira dodatni proces dete. Proces roditelj sa standardnog ulaza cita 5 brojeva i upisuje ih u deljenu memoriju.
 * Nakon toga proces dete cita brojeve iz deljene memorije, sabira ih i upisuje u deljenu memoriju. Proces roditelj cita zbir i prikazuje ga
 * na ekranu, a zatim se postupak ponavlja. Postupak se ponavlja sve dok suma brojeva nije jednaka nuli i kada proces dete i roditelj izlaze.
 * Za sinhronizaciju procesa koristiti System V Semafore.*/

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

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int main() {
    int procaid, procbid;
    union semun semopts;
    struct sembuf sem_lock = {0, -1, 0};
    struct sembuf sem_unlock = {0, 1, 0};

    // Kreiranje semafora
    procaid = semget((key_t)PROC_A_KEY, 1, 0666 | IPC_CREAT);
    procbid = semget((key_t)PROC_B_KEY, 1, 0666 | IPC_CREAT);

    // Inicijalizacija semafora
    semopts.val = 1;
    semctl(procaid, 0, SETVAL, semopts);
    semopts.val = 0;
    semctl(procbid, 0, SETVAL, semopts);

    // Kreiranje procesa
    if (fork() != 0) {  // Proces roditelj
        int shm_id = shmget(MEM_KEY, 5 * sizeof(int), IPC_CREAT | 0666);  // Deljena memorija za 5 brojeva
        int* shm_ptr = shmat(shm_id, NULL, 0);  // Priključivanje na deljenu memoriju

        int numbers[5], sum;

        do {
            // Upis brojeva u deljenu memoriju
            semop(procaid, &sem_lock, 1);  // Zaključavanje roditeljskog semafora

            printf("Unesite 5 brojeva: ");
            for (int i = 0; i < 5; i++) {
                scanf("%d", &numbers[i]);
            }

            memcpy(shm_ptr, numbers, 5 * sizeof(int));  // Kopiranje brojeva u deljenu memoriju

            semop(procbid, &sem_unlock, 1);  // Otključavanje semafora za dete

            // Čitanje zbira iz deljene memorije
            semop(procaid, &sem_lock, 1);  // Čekanje da dete izračuna zbir

            sum = shm_ptr[0];  // Prvi element deljene memorije sada sadrži zbir
            printf("Zbir brojeva je: %d. Izlazim...\n", sum);

            semop(procbid, &sem_unlock, 1);  // Otključavanje za novu iteraciju deteta

        } while (sum != 0);  // Program se prekida kad je zbir 0

        // Oslobađanje resursa
        shmdt(shm_ptr);
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(procaid, 0, IPC_RMID, NULL);
        semctl(procbid, 0, IPC_RMID, NULL);

    } else {  // Proces dete
        int shm_id = shmget(MEM_KEY, 5 * sizeof(int), IPC_CREAT | 0666);  // Deljena memorija za 5 brojeva
        int* shm_ptr = shmat(shm_id, NULL, 0);  // Priključivanje na deljenu memoriju

        int sum;

        do {
            // Čitanje brojeva iz deljene memorije
            semop(procbid, &sem_lock, 1);  // Čekanje na roditeljski unos

            sum = 0;
            for (int i = 0; i < 5; i++) {
                sum += shm_ptr[i];  // Sabiranje brojeva
            }

            shm_ptr[0] = sum;  // Upisivanje zbira nazad u deljenu memoriju

            semop(procaid, &sem_unlock, 1);  // Obaveštavanje roditelja da je zbir spreman

        } while (sum != 0);  // Prekida se kad je zbir 0

        // Oslobađanje resursa
        shmdt(shm_ptr);
    }

    return 0;
}
