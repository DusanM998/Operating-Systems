/*Linux program koji sadrzi bafer u koji se mogu smestiti 2 int broja. Zasebna nit periodicno generise 2 broja(u opsegu od 1 do 10) i upisuje ih u bafer. Kada se u baferu nadje novi par brojeva glavna nit(main f-ja) treba da odredi zbir da 2 broja i odstampa ga na std izlazu. Vremenski razmak izmedju 2 uzastopna upisa u bafer je slucajna vrednost izmedju 0 i 5 sek.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

/*Deljeni bafer*/
int buffer[2];

/*Mutex i uslovna promenljiva*/
pthread_mutex_t mutex;
pthread_cond_t condVarFull;
pthread_cond_t condVarEmpty;
int bufferFull = FALSE;

void* threadFunc(void* arg)
{
    /*Pauza izmedju 2 upisa u bafer*/
    long int rnd; //1
    float normalisedRnd; //2
    int sleepTime; //3; 1,2,3 su promenljive koje nam signaliziraju koliko ce nit da ceka
    int i;
    int bufferPointer;
    
    bufferPointer = 0;
    
    for(i=1; i <= 10; i++)
    {
        rnd = random();
        normalisedRnd = (float)rnd / (float)RAND_MAX;
        sleepTime = (int)(normalisedRnd * 5);
        pthread_mutex_lock(&mutex);
        
        while(bufferFull)
            pthread_cond_wait(&condVarEmpty, &mutex);
        
        buffer[bufferPointer] = i;
        printf("Thread je u buffer[%d] upisao vrednost %d\n", bufferPointer, i);
        
        bufferPointer = (bufferPointer + 1) % 2;
        if(bufferPointer == 0)
            bufferFull = TRUE;
        
        pthread_cond_signal(&condVarFull);
        pthread_mutex_unlock(&mutex);
        sleep(sleepTime);
    }
}
    int main()
    {
        pthread_t threadID;
        int i;
        
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&condVarEmpty, NULL);
        pthread_cond_init(&condVarFull, NULL);
        
        /*Kreira nit okja generise brojeve i upisuje ih u bafer*/
        pthread_create(&threadID, NULL, threadFunc, NULL);
        
        for(i=0; i < 5; i++)
        {
            pthread_mutex_lock(&mutex);
            
            /*Nit ceka dok se bafer ne napuni sa 2 broja*/
            while(!bufferFull)
                pthread_cond_wait(&condVarFull, &mutex); //Ceka signal od niti
                
            /*Stampa zbir brojeva iz bafera*/
            printf("Zbir brojeva iz bafera: %d + %d = %d\n", buffer[0],buffer[1],buffer[0]+buffer[1]);
            bufferFull = FALSE;
            pthread_cond_signal(&condVarEmpty); //signal da je bafer prazan
            pthread_mutex_unlock(&mutex);
        }
        
        /*Brise uslovnu promenljivu i mutex*/
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condVarFull);
        pthread_cond_destroy(&condVarEmpty);
        
        return 0;
    }
