/*Linux program koji u datoteku izlaz.txt upisuje niz od prvih 20 parnih i 20 neparnih brojeva redom. Parne brojeve upisuje
 * jedna nit, a neparne druga nit. Sinhronizacijom niti semaforima obezbediti da se u datoteku parni i neparni brojevi upisuju naizmenicno*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 20

sem_t sem_parni;
sem_t sem_neparni;

FILE* file;

void *threadParni(void *arg)
{
    for(int i=2; i <= N *2; i+=2)
    {
        sem_wait(&sem_parni); //Ceka dok ne dobije dozvolu za upis parnog broja
        fprintf(file, "Nit 1 upisala parni broj: %d\n", i);
        sem_post(&sem_neparni); //Dozvoli upis neparnom broju
    }
    return NULL;
}

void *threadNeparni(void *arg)
{
    for(int i=1; i< N * 2; i+=2)
    {
        sem_wait(&sem_neparni); //Ceka dok ne dobije dozvolu za upis neparnog broja
        fprintf(file, "Nit 2 upisala neparni broj: %d\n", i);
        sem_post(&sem_parni);
    }
    return NULL;
}

int main()
{
    pthread_t nit_parni, nit_neparni;
    
    file = fopen("izlaz.txt", "w");
    if(file == NULL)
    {
        perror("Greska pri otvaranju fajla!\n");
        return 1;
    }
    
    //Inicijalizacija semafora
    sem_init(&sem_parni, 0, 1); //Pocetno stanje za parne brojeve je 1 - moze odmah upisivati
    sem_init(&sem_neparni, 0, 0); //Pocetno stanje za neparne brojeve je 0 - mora cekati
    
    //Kreiranje niti
    pthread_create(&nit_parni, NULL, threadParni, NULL);
    pthread_create(&nit_neparni, NULL, threadNeparni, NULL);
    
    //Cekanje da se niti zavrse
    pthread_join(nit_parni, NULL);
    pthread_join(nit_neparni, NULL);
    
    //Zatvaranje datoteke i oslobadjanje resursa
    fclose(file);
    sem_destroy(&sem_parni);
    sem_destroy(&sem_neparni);
    
    printf("Zavrseno je upisivanje u datoteku izlaz.txt!\n");
    
    return 0;
}
