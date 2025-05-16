/*Linux program koji kreira dve dodatne niti. Program treba da odstampa cele brojeve redom od 0 do N,
 * pri cemu se N prosledjuje programu kao argument komandne linije. Pri tome brojeve deljive sa 7 stampa prva nit,
 * a ostale brojeve druga nit. Program izlazi posto niti odstampaju sve brojeve.*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n;

void* deljiviSa7(void* arg)
{
    for(int i=0; i<=n;i++)
    {
        if(i%7==0)
        {
            printf("Nit 1 stampa: %d\n", i);
            sleep(2);
        }
    }
    return NULL;
}

void* nedeljiviSa7(void* arg)
{
    for(int i=0; i <=n;i++)
    {
        if(i&7 !=0)
        {
            printf("Nit 2 stampa: %d\n", i);
        }
        sleep(2);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }
    
    n = atoi(argv[1]);
    
    pthread_t thread1, thread2;
    
    //Kreiranje prve niti
    pthread_create(&thread1, NULL, deljiviSa7, NULL);
    
    //Kreiranje druge niti
    pthread_create(&thread2, NULL, nedeljiviSa7, NULL);
    
    //Cekanje da se obe niti zavrse
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    return 0;
}
