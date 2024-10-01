/*Linux program koji sadrzi bafer u koji se moze smestiti 10 integer brojeva. Jedna dodatna nit(worker thread) generise
 * 5 slucajnih brojeva (u opsegu od 0 do 299) i upisuje ih na mesta sa parnim indeksima u ovaj bafer. Druga dodatna nit
 * (worker thread) istovremeno generise 5 slucajnih brojeva(u opsegu od 300 do 499) i upisuje ih na mesta sa neparnim indeksima
 * u ovaj bafer. Kada se u baferu nadje nova grupa brojeva(na parnim i na neparnim indeksima), glavna nit(main f-ja)
 * treba da odredi da li je zbir tih brojeva veci od 2000 i odstampa adekvatnu poruku na standardnom izlazu. Ovaj postupak
 * generisanja 10 brojeva i ispitivanje vrednosti njihovog zbira treba da se ponovi 5 puta. Adekvatno sinhronizovati rad niti
 * koje generisu brojeve i glavne niti koja proverava njihov zbir.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 10
#define NUM_ITERATIONS 5

int buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
pthread_cond_t cond_main;
pthread_cond_t condParnih;
pthread_cond_t condNeparnih;

int parnaNit = 0;
int neparnaNit = 0;
int runde = 0;

void *parni_worker(void *arg)
{
    for(int i=0; i < 5; i++)
    {
        sleep(1);
        
        pthread_mutex_lock(&mutex);
        
        //Ceka dok prva nit ne izgenerise parne brojeve
        while(parnaNit != 0 || neparnaNit !=0)
            pthread_cond_wait(&condParnih, &mutex);
        
        printf("Prva nit generise random brojeve u opsegu od 0 do 299: \n");
        for(int i=0; i < BUFFER_SIZE; i+=2)
        {
            buffer[i] = rand() % 300; //Generise random brojeve od 0 do 299
            printf("Parna nit generisala: %d na poziciji %d\n", buffer[i], i);
        }
        printf("\n");
        
        parnaNit = 1;
        
        pthread_cond_signal(&condNeparnih);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *neparni_worker(void *arg)
{
    for(int i=0; i < 5; i++)
    {
        sleep(1);
        
        pthread_mutex_lock(&mutex);
        //Ceka dok druga nit ne izgenerise neparne brojeve
        while(parnaNit == 0 || neparnaNit !=0)
            pthread_cond_wait(&condNeparnih, &mutex);
        
        printf("Druga nit generise random brojeve u opsegu od 300 do 499: \n");
        for(int i=1; i < BUFFER_SIZE; i+=2)
        {
            buffer[i] = 300 + rand() % 200; //Generise random brojeve od 0 do 299
            printf("Neparna nit generisala: %d na poziciji %d\n", buffer[i], i);
        }
        printf("\n");
        
        neparnaNit = 1;
        
        pthread_cond_signal(&cond_main);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t nit1, nit2;
    int sum;
    
    srand(time(NULL));
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condParnih, NULL);
    pthread_cond_init(&condNeparnih, NULL);
    
    pthread_create(&nit1, NULL, parni_worker, NULL);
    pthread_create(&nit2, NULL, neparni_worker, NULL);
    
    for(int i=0; i < 5; i++)
    {
        printf("%d. prolaz:\n", i+1);
        pthread_mutex_lock(&mutex);
        
        while(parnaNit == 0 || neparnaNit == 0)
            pthread_cond_wait(&cond_main, &mutex);
        
        for(int i = 0; i < BUFFER_SIZE; i++)
            sum+= buffer[i];
        
        if(sum > 2000)
            printf("Glavna nit: Suma %d je veca od 2000! Izlazim...\n", sum);
        else
            printf("Glavna nit: Suma %d je manja od 2000!\n", sum);
        
        parnaNit = 0;
        neparnaNit = 0;
        
        pthread_cond_signal(&condParnih);
        pthread_mutex_unlock(&mutex);
    }
    
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condParnih);
    pthread_cond_destroy(&condNeparnih);
    
    return 0;
}
