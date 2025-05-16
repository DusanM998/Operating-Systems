#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int global=5;

void funkcijaNiti(void* arg)
{
    printf("Ja sam nit procesa: %d\n", getpid());
    printf("Globalni podatak vidim kao: %d\n", global);
    
    global = 10;
    
    printf("Nit: globalni podatak je: %d\n",global);
}

int main(int argc, char* argv[])
{
    pthread_t nit;
    pthread_create(&nit, NULL, (void*)funkcijaNiti, NULL);
    
    printf("Ja sam osnovna nit procesa: %d\n", getpid());
    printf("I ja vidim globalni podatak kao: %d\n", global);
    
    global = 15;
    pthread_join(nit, NULL);
    
    printf("Glavna nit: globalni podatak je: %d\n", global);
    
    return 0;
}
    
