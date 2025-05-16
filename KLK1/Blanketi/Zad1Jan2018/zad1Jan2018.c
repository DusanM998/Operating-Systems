/*Linux program u kome 5 niti pristupa deljivom celobrojnom nizu. Cetvrta nit periodicno (na 3s) slucajno odabranom elementu niza dodaje slucajno generisanu celobrojnu vrednost iz opsega [-10 : 10]. Poslednja nit stampa elemente niza samo ukoliko je njihova suma(nakon izmene od strane ostalih niti) paran broj. Za sinhronizaciju niti iskoristiti mehanizam mutex-a i uslovnih prom.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

#define MAX_ELEMENTS 5

pthread_mutex_t mutex;
pthread_cond_t uslovna;

int sharedResources[MAX_ELEMENTS];

int parno = 0;
int brIzmena = 0;

void* generatorThread(void* arg)
{
    int i=0;
    int rbrNiti = *((int*)arg);
    int index;
    int vrednost1;
    int vrednost2;
    int vrednost;
    int suma;
    
    while(i < 100) //Konacan broj generisanja
    {
        pthread_mutex_lock(&mutex);
        
        index = rand()%MAX_ELEMENTS;
        vrednost1 = rand()%11;
        vrednost2 = rand()%11;
        vrednost = vrednost1 + vrednost2;
        
        printf("Nit: %d dodaje na poziciji: %d, vrednost: %d\n",rbrNiti,index,vrednost);
        fflush(stdout);
        brIzmena = (brIzmena+1);
        sharedResources[index]+=vrednost;
        
        suma = 0;
        for(int i=0; i < MAX_ELEMENTS; i++)
        {
            suma+=sharedResources[i];
        }
        
        if(brIzmena == 4)
            printf("%d\n", suma);
        
        if(suma%2 == 0 && brIzmena == 4)
        {
            parno = 1;
            brIzmena = 0;
            pthread_cond_signal(&uslovna);
        }
        
        if(brIzmena == 4)
            brIzmena = 0;
        
        pthread_mutex_unlock(&mutex);
        sleep(3);
        i++;
    }
}

void* consumerThread(void* arg)
{
    int i=0;
    while(i < 100)
    {
        pthread_mutex_lock(&mutex);
        
        while(parno == 0)
            pthread_cond_wait(&uslovna, &mutex);
        
        for(int i=0; i < MAX_ELEMENTS; i++)
            printf("%d", sharedResources[i]);
        
        fflush(stdout);
        printf("\n");
        
        parno = 0;
        i++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[])
{
    pthread_t niti[5];
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&uslovna, NULL);
    int indexNiti[5] = {0, 1, 2, 3, 4};
    
    for(int i=0; i < MAX_ELEMENTS; i++)
        sharedResources[i] = 0;
        
    pthread_create(&niti[4], NULL, (void*)consumerThread, NULL);
    
    for(int i=0; i < 4; i++)
        pthread_create(&niti[i], NULL, (void*)generatorThread, &indexNiti[i]);
    
    pthread_join(niti[0], NULL);
    pthread_join(niti[1], NULL);
    pthread_join(niti[2], NULL);
    pthread_join(niti[3], NULL);
    pthread_join(niti[4], NULL);
    
    exit(0);
}
