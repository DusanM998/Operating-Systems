/*Linux program koji sadrzi bafer u koji se mogu smestiti dve celobrojne vrednosti. Zasebna nit periodicno generise dva slucajna broja
 * (u opsegu od 100 do 199) i upisuje ih u ovaj bafer. Kada se u baferu nadje novi par brojeva, glavna nit(main f-ja) treba da odredi da li su
 * ta dva broja jednaka i da odstampa adekvatnu poruku na standardnom izlazu. Ovaj postupak generisanja para brojeva i ispitivanje njihobve jednakosti
 * treba da se ponovi 10 puta.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 2
#define NUM_OF_ITERATIONS 10
#define TRUE 1
#define FALSE 0

int buffer[2];
pthread_mutex_t mutex;
pthread_cond_t condVarFull;
pthread_cond_t condVarEmpty;
int bufferFull = FALSE;

void *generisiBrojeve(void *arg)
{
    for(int i=0; i< 10; i++)
    {
        
        //sleep(2);
            
        pthread_mutex_lock(&mutex);
            
        int num1 = 100 + rand() % 100;
        int num2 = 100 + rand() % 100;
            
        while(bufferFull)
            pthread_cond_wait(&condVarEmpty, &mutex);
            
        buffer[0] = num1;
        buffer[1] = num2;
        printf("Nit 1. generise random brojeve u opsegu od 100 do 199...\n");
        printf("Nit 1. u buffer upisao vrednosti %d i %d:\n", num1, num2);
        
        bufferFull = TRUE;
            
        //Signalizira glavnoj niti da je zavrseno generisanje
        pthread_cond_signal(&condVarFull);
            
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t nit;
    int i;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condVarEmpty, NULL);
    pthread_cond_init(&condVarFull, NULL);
    
    pthread_create(&nit, NULL, generisiBrojeve, NULL);
    
    for(i=0; i < 10; i++)
    {
        printf("%d. prolaz zapocet.\n", i+1);
        pthread_mutex_lock(&mutex);
        
        while(!bufferFull)
            pthread_cond_wait(&condVarFull, &mutex);
        
        if(buffer[0] == buffer[1])
        {
            printf("Par brojeva u baferu %d i %d su jednaki!\n", buffer[0], buffer[1]);
        }
        else
        {
            printf("Par brojeva u baferu %d i %d nisu jednaki!\n", buffer[0], buffer[1]);
        }
            
        bufferFull = FALSE;
            
        //Signalizira radnoj niti da moze da generise novu grupu brojeva
        pthread_cond_signal(&condVarEmpty);
        pthread_mutex_unlock(&mutex);
        
        printf("%d. prolaz zavrsen.\n", i+1);
        printf("\n");
    }
    
    pthread_join(nit, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condVarFull);
    pthread_cond_destroy(&condVarEmpty);

    return 0;
}
