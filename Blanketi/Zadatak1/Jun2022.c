/*Linux program koji iz tekst. datoteke "niz.txt" ucitava niz celih brojeva(svaki broj u zasebnoj liniji) u niz u memoriji.
 * Glavni program odmah po startovanju kreira dodatnu nit koja sortira brojeve ucitane u niz u memoriji.
 * Po zavrsetku sortiranja glavni program stampa sortirani niz*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *niz = NULL;
int N = 0;
pthread_mutex_t mutex;

void* sortirajNiz(void* arg)
{
    pthread_mutex_lock(&mutex);
    
    for(int i=0; i < N - 1; i++)
    {
        for(int j=0; j < N - i - 1; j++)
        {
            if(niz[j] > niz[j+1])
            {
                int tmp = niz[j];
                niz[j] = niz[j+1];
                niz[j+1] = tmp;
            }
        }
    }
    
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main()
{
    FILE* file = fopen("niz.txt","r");
    if(!file)
    {
        perror("Ne moze se otvoriti fajl!\n");
        return 1;
    }
    
    int number;
    int capacity = 10;
    niz = (int*)malloc(capacity * sizeof(int));
    if(!niz)
    {
        perror("Neuspesna alokacija memorije!\n");
        fclose(file);
        return 1;
    }
    
    while(fscanf(file, "%d", &number) == 1)
    {
        if(N == capacity)
        {
            capacity *= 2;
            niz = (int*)realloc(niz, capacity * sizeof(int));
            if(!niz)
            {
                perror("Neuspesna alokacija memorije!\n");
                fclose(file);
                return 1;
            }
        }
        niz[N++] = number;
    }
    
    fclose(file);
    
    pthread_t sort_thread;
    pthread_mutex_init(&mutex, NULL);
    
    //Kreira se nit za sortiranje
    pthread_create(&sort_thread, NULL, sortirajNiz, NULL);
    
    //Ceka da se niti zavrse
    pthread_join(sort_thread, NULL);
    
    printf("Sortirani niz: \n");
    for(int i=0; i < N; i++)
        printf("%d ", niz[i]);
    printf("\n");
    
    //Oslobadja resurse
    free(niz);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
