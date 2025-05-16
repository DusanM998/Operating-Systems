#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t binsem;

void* prvaFunkcija(void* arg)
{
    pthread_mutex_lock(&binsem);
    printf("Ja sam prva nit i druga nit mora da ceka!\n");
    sleep(5);
    pthread_mutex_unlock(&binsem);
}

void* drugaFunkcija(void* arg)
{
    pthread_mutex_lock(&binsem);
    printf("Ja sam druga nit i prva nit mora da ceka!\n");
    sleep(5);
    pthread_mutex_unlock(&binsem);
}

int main(int argc, char* argv[])
{
    pthread_t prva;
    pthread_t druga;
    
    pthread_mutex_init(&binsem, NULL);
    
    pthread_create(&prva, NULL, (void*)prvaFunkcija, NULL);
    pthread_create(&druga, NULL, (void*)drugaFunkcija, NULL);
    
    pthread_join(prva, NULL);
    pthread_join(druga, NULL);
    
    pthread_mutex_destroy(&binsem);
    
    exit(0);
}
