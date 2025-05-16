/*Sa uslovnim sinhronizacijom izmedju niti*/
/*Linux program koji kreira dve dodatne niti. Program treba da odstampa cele brojeve redom od 0 do N,
 * pri cemu se N prosledjuje programu kao argument komandne linije. Pri tome brojeve deljive sa 7 stampa prva nit,
 * a ostale brojeve druga nit. Program izlazi posto niti odstampaju sve brojeve.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n;
int current = 0; //Trenutni broj koji treba da se stampa
pthread_mutex_t mutex;
pthread_cond_t cond1, cond2;

void* deljiviSa7(void* arg)
{
    while(1)
    {
        //Pristupanje deljivom resursu u KS
        pthread_mutex_lock(&mutex);
        
        while(current <= n && current % 7 !=0)
        {
            pthread_cond_wait(&cond1, &mutex);
        }
        
        if(current > n)
        {
            pthread_cond_signal(&cond2);
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        printf("Nit 1 stampa: %d\n", current);
        current++;
        
        pthread_cond_signal(&cond2);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* nedeljiviSa7(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        
        while(current <= n && current % 7 ==0)
        {
            pthread_cond_wait(&cond2, &mutex);
        }
        
        if(current > n)
        {
            pthread_cond_signal(&cond1);
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        printf("Nit 2 stampa: %d\n", current);
        current++;
        
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Nepravilno zadati parametri! Zadati ih u formatu: %s <N>\n", argv[0]);
        return 1;
    }
    
    n = atoi(argv[1]);
    
    pthread_t thread1, thread2;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    
    //Kreiranje prve niti
    pthread_create(&thread1, NULL, deljiviSa7, NULL);
    
    //Kreiranje druge niti
    pthread_create(&thread2, NULL, nedeljiviSa7, NULL);
    
    //Cekanje da se obe niti zavrse
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&cond1);
    pthread_mutex_destroy(&cond2);
    
    return 0;
}
