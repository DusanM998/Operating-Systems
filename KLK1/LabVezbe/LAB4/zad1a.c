#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

/*Niz koji sadrzi recenice*/
char reci[N][20] = {"Ovo ", "je ", "test ", "za ", "semafore"};

/*Niz od 4 semafora*/
sem_t sems[N-1];

/*Funkcija niti*/
void* stampanje(void* arg)
{
    /*Ulazni argument je struktura argument*/
    int p = *((int *) arg);
    /*Samo prva nit odmah stampa svoju rec, ostale moraju da sacekaju prethodnu*/
    if(p>0)
        sem_wait(&sems[p-1]);
    printf("%s", reci[p]);
    
    if(p < 4) //Zato sto poslednja nit nema kome da signalizira da moze da odstampa svoju rec
        sem_post(&sems[p]);
    sleep(3);
}

int main()
{
    int i;
    
    /*Niz ulaznih argumenata niti*/
    int args[N];
    
    /*Niz identifikatora niti*/
    pthread_t niti[N];
    
    /*Inicijalizacija semafora*/
    for(i=0; i < N-1; i++)
        sem_init(&sems[i], 0, 0);
    
    /*Kreiramo niti*/
    for(i=0; i < N; i++)
    {
        args[i] = i;
        pthread_create(&niti[i], NULL, (void*)stampanje, (void*)&args[i]);
    }
    
    /*Main f-ja ceka da se sve niti zavrse*/
    for(i=0; i < N; i++)
        pthread_join(niti[i], NULL);
    return 0;
}
