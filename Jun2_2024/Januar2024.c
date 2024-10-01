/*Linux program koji iz text. datoteke "ulaz.txt" ucitava nizove celih brojeva jedan po jedan (svaki niz u zasebnoj liniji, 
 * pri cemu su brojevi u liniji razdvojeni blanko znacima) u niz u memoriji. Glavni program odmah po startovanju kreira dodatnu nit
 * koja sumira brojeve ucitane u niz u memoriji. Po zavrsetku sumiranja, glavni program stampa sumu i prelazi na naredni ucitani niz.
 * Postupak se ponavlja sve dok se ne procitaju i sumiraju svi nizovi iz datoteke.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_LINE_LENGHT 1024

int *array = NULL; //Dimanicki alocirani niz za brojeve
int N;
int sum = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;
int ready_to_sum = 0; //Signalizira da je niz spreman za sumiranje

void* sumirajNiz(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        
        while(!ready_to_sum)
            pthread_cond_wait(&cond, &mutex);
        
        //Sumiranje niza
        sum = 0;
        for(int i=0; i < N; i++)
            sum+=array[i];
        
        ready_to_sum = 0;
        
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void process_line(char* line)
{
    //Parsira liniju i ucitava brojeve u niz
    char* token = strtok(line, " ");
    N = 0;
    
    while(token != NULL)
    {
        array[n++] = atoi(token);
        token = strtok(NULL, " ");
    }
    
    //Signalizira da je niz spreman za sumiranje
    pthread_mutex_lock(&mutex);
    ready_to_sum = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    
    //Ceka da nit zavrsi sumiranje
    pthread_mutex_lock(&mutex);
    while(ready_to_sum)
        pthread_cond_wait(&cond, &mutex);
    
    pthread_mutex_unlock(&mutex);
    
    printf("Suma brojeva ucitanih iz fajla: %d\n", sum);
}

int main()
{
    FILE* file = fopen("ulaz.txt", "r");
    if(!file)
    {
        perror("Ne moze se otvoriti fajl, ne postoji!\n");
        return 1;
    }
    
    char line[MAX_LINE_LENGHT];
    pthread_t sum_thread;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    //Alociranje memorije za niz
    array = (int*)malloc(MAX_LINE_LENGHT / 2 * sizeof(int));
    if(!array)
    {
        perror("Neuspesna alokacija memorije!\n");
        fclose(file);
        return 1;
    }
    
    //Kreira nit za sumiranje
    pthread_create(&sum_thread, NULL, sum_array, NULL);
    
    //Obradjuje svaku liniju u datoteci
    while(fgets(line, sizeof(line), file)){
        process_line(line)
    }
    
    //Oslobadja resurse
    free(array);
    fclose(file);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
