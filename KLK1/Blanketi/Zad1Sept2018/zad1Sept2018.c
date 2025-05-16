/*Septembar2018 i Septembar2020*/
/*Linux program koji sadrzi bafer u koji se mogu smestiti 2 celobrojne vrednosti. Zasebna nit periodicno generise 2 slucajna broja (u opsegu od 100 do 199) i upisuje ih u ovaj bafer. Kada se u baferu nadje novi par brojeva, glavna nit(main f-ja) treba da odredi da li su ta 2 broja jednaka i odstampa adekvatnu poruku na std izlazu. Ovaj postupak generisanja para brojeva i ispitivanje njihove jednakosti treba da se ponovi 10 puta*/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int bafer[2];
int count = 0;
pthread_cond_t uslovna;
pthread_mutex_t mutex;

void* funkicjaNiti(void* arg)
{
    int i=0;
    while(i < 20)
    {
        pthread_mutex_lock(&mutex);
        
        if(count == 0)
        {
            bafer[count] = (rand() % 100) + 100;
            printf("Nit: dodajem vrednost %d na poziciji %d\n", bafer[count], count);
            count++;
        }
        else
        {
            bafer[count] = (rand()%100) + 100;
            printf("Nit: dodajem vrednost %d na poziciji %d\n", bafer[count], count);
            pthread_cond_signal(&uslovna);
        }
        
        i++;
        pthread_mutex_unlock(&mutex);
        
        sleep(2); //periodicno genesiranje na 2s
        
    }
}

int main(int argc, char* argv[])
{
    pthread_t nit;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&uslovna, NULL);
    
    int i=0;
    pthread_create(&nit, NULL, (void*)funkicjaNiti, NULL);
    while(i < 10)
    {
        pthread_mutex_lock(&mutex);
        
        while(count != 1)
            pthread_cond_wait(&uslovna, &mutex);
        if(bafer[0] == bafer[1])
            printf("Glavna nit: Brojevi su jednaki!\n");
        else
            printf("Glavna nit: Brojevi nisu jednaki!\n");
        
        count = 0;
        
        i++;
        
        pthread_mutex_unlock(&mutex);
    }
}
