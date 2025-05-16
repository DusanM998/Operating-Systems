/*Linux program koji sadrzi bafer u koji se moze smestiti 10 integer brojeva. Jedna zasebna nit, kreirana prilikom startovanja programa,
 * periodicno generise 10 slucajnih brojeva(u opsegu od 0 do 199) i upisuje ih u ovaj bafer. Kada se u baferu nadje nova grupa brojeva,
 * glavna nit(main f-ja) treba da odredi da li je zbir tih brojeva veci od 800 i odstampa adekvatnu poruku na standardnom izlazu.
 * Ovaj postupak generisanja 10 brojeva i ispitivanje vrednosti njihovog zbira treba da se ponavlja sve dok sracunat zbir ne predje 1000.
 * Adekvatno sinhronizovati rad niti koja generise brojeve i glavne niti koja proverava njihov zbir*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define MAX_SUM 1000
#define CHECK_SUM 800

int buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
pthread_cond_t cond_generate_complete;
int sum = 0;
int current_sum = 0;
int generation_complete = 0;

//Funkcija koja generise brojeve i upisuje ih u bafer
void *generateNumbers(void *arg)
{
    srand(time(NULL) ^ (pthread_self() << 16));
    
    while(1)
    {
        pthread_mutex_lock(&mutex);
        
        printf("Nit 1. generise 10 slucajnih brojeva od 0 do 199...\n");
        for(int i=0; i < BUFFER_SIZE; i++)
        {
            buffer[i] = rand() % 200;
            printf("%d ", buffer[i]);
        }
        printf("\n");
        
        //Obracunava trenutni zbir
        current_sum = 0;
        for(int i=0; i < BUFFER_SIZE; i++)
            current_sum += buffer[i];
        generation_complete = 1;
        
        pthread_cond_signal(&cond_generate_complete);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    
    return NULL;
}

int main()
{
    pthread_t generate_thread;
    
    pthread_create(&generate_thread, NULL, generateNumbers, NULL);
    
    while(sum <= MAX_SUM)
    {
        pthread_mutex_lock(&mutex);
        
        while(!generation_complete)
            pthread_cond_wait(&cond_generate_complete, &mutex);
        
        //Glavna nit obradjuje brojeve kada su generisani
        printf("Glavna nit: Suma trenutnih brojeva: %d\n", current_sum);
        
        sum += current_sum;
        
        if(current_sum > 800)
            printf("Glavna nit: Suma je veca od 800!\n");
        else
            printf("Glavna nit: Suma je manje od 800!\n");
        
        generation_complete = 0;
        
        pthread_mutex_unlock(&mutex);
    }
    
    pthread_cancel(generate_thread);
    pthread_join(generate_thread, NULL);
    
    printf("Glavna nit: Krajnja suma: %d\n", sum);
    printf("Krajnja suma: %d > 1000! KRAJ\n", sum);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_generate_complete);
    
    return 0;
}
