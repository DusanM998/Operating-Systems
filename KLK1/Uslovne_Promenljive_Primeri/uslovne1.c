#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

pthread_mutex_t sem;
pthread_cond_t uslovna;
int x=0;

void* prvaFunkcija(void* args)
{
    pthread_mutex_lock(&sem);
    while(x < 100)
    {
        printf("Ja sam prva nit i blokiram se na uslovnoj promeljivoj!\n");
        pthread_cond_wait(&uslovna, &sem);
    }
    printf("Ja sam prva nit u upravo sam odblokirana!\n");
    pthread_mutex_unlock(&sem);
}

void* drugaFunkcija(void* args)
{
    pthread_mutex_lock(&sem);
    while(x < 200)
    {
        x++;
        if(x == 100)
        {
            printf("Ja sam druga nit i saljem uslov za aktiviranje druge niti!\n");
            pthread_cond_signal(&uslovna);
        }
    }
    printf("Ja sam druga nit i izlazim iz kriticne sekcije\n!");
    pthread_mutex_unlock(&sem);
}

int main(int argc, char* argv[])
{
    pthread_t nit1;
    pthread_t nit2;
    
    pthread_mutex_init(&sem, NULL);
    pthread_cond_init(&uslovna, NULL);
    
    pthread_create(&nit1, NULL, (void*)prvaFunkcija, NULL);
    pthread_create(&nit2, NULL, (void*)drugaFunkcija, NULL);
    
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    
    pthread_mutex_destroy(&sem);
    pthread_cond_destroy(&uslovna);
    
    exit(0);
}
