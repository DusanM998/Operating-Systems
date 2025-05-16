/*Linix program koji simulira komunikaciju izmedju niti koriscenjem celobrojnih bafera. Nit N0 generise celobrojne podatke i upisuje ih u bafere B1, B2 i B3. Prilikom upisivanja podataka, nit N0 najpre proverava da li u baferu B3 ima slobodnih pozicija i u tom slucaju novi podatak upisuje u bafer B3. Ako je bafer B3 pun, proverava da li postoje slobodne pozicije u baferu B1. Ako postoje, proizvodjac N0 generisani podatak upisuje u bafer B1. Ukoliko je i bafer B1 pun, novi podatak se upisuje u bafer B2, ukoliko on poseduje slobodne pozicije. Ukoliko ni u jednom baferu nema slobodnih pozicija, nit N0 ceka, dok se u nekom od bafera ne oslobodi pozicija za upis novog elementa. Niti N1, N2, N3 citaju podatke iz bafera B1, B2, B3 i prikazuju ih na std izlazu. za sinhronizaciju izmedju procesa koristiti POSTFIC semafore.*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>

/*Deklaracije globalnih prom. koje su vidljive u svim f-jama, pa i u f-jama niti*/
/*Dimenzije bafera B1, B2 i B3*/
#define N1 10
#define N2 5
#define N3 20

/*Celobrojni baferi B1, B2, B3*/
int B1[N1], B2[N2], B3[N3];

/*Brojaci slobodnih pozicija u baferima N1, N2 i N3*/
int empty1 = N1;
int empty2 = N2;
int empty3 = N3;

/*Indekxi pozicija za citanje i pisanje u bafer. Baferi B1, B2 i B3 ne ponasaju se kao FIFO baferi odnosno mora da postoji i glava i rep*/
int head1 = 0;
int tail1 = 0;
int head2 = 0;
int tail2 = 0;
int head3 = 0;
int tail3 = 0;

/*Semafori za medjusobno izkljucenje*/
sem_t mutex_B1, mutex_B2, mutex_B3, mutex_empty;

void* nit0(void* arg)
{
    int data;
    while(1)
    {
        /*Pravi pauzu pre generisanja sledeceg pod*/
        sleep(rand() %3);
        /*Nit N0 proverava da li bar u jendom baferu postoji prazna pozicija*/
        /*Ukoliko ne postoji, nit N0 se blokira dok ne dobija signal da se oslobodila bar jedna pozicija*/
        sem_wait(&mutex_empty);
        
        printf("Generisanje podataka!\n");
        
        /*Pristupanje svakom baferu ide u zasebnoj kriticnoj sekciji*/
        
        /*Podatak pokusava najpre da upise u bafer B3*/
        sem_wait(&mutex_B3);
        if(empty3 > 0)
        {
            B3[tail3] = rand() % 1000;
            tail3 = (tail3 + 1) % N3;
            empty3--;
            sem_post(&mutex_B3);
            continue;
        }
        else
            sem_post(&mutex_B3);
        
        /*Zatim pokusava da upise u B1, ako ima slobodnih pozicija*/
        sem_wait(&mutex_B1);
        if(empty1 > 0)
        {
            B1[tail1] = rand() % 1000;
            tail1 = (tail1 + 1) % N1;
            empty1--;
            sem_post(&mutex_B1);
            continue;
        }
        else
            sem_post(&mutex_B1);
        
        /*Na kraju upisuje u B2*/
        sem_wait(&mutex_B2);
        if(empty2 > 0)
        {
            B2[tail2] = rand() % 1000;
            tail2 = (tail2 + 1) % N2;
            empty2--;
            sem_post(&mutex_B2);
        }
        else
            sem_post(&mutex_B2);
    }
}

void* nit1(void* arg)
{
    while(1)
    {
        /*Baferu se pristupa u kriticnoj sekciji*/
        sem_wait(&mutex_B1);
        
        /*Podatak se cita samo ukoliko bafer nije prazan*/
        if(empty1 < N1)
        {
            printf("Nit N1 je ocitala podatak %d\n", B1[head1]);
            head1 = (head1 + 1) % N1;
            empty1++;
            
            /*Obavestava N0 da se pojavila nova slobodna pozicija*/
            sem_post(&mutex_empty);
        }
        sem_post(&mutex_B1);
        
        /*Pravo pauzu pre citanja sledeceg podataka*/
        sleep(rand() % 5);
    }
}

void* nit2(void* arg)
{
    while(1)
    {
        /*Baferu se pristupa u kriticnoj sekciji*/
        sem_wait(&mutex_B2);
        
        /*Podatak se cita samo ukoliko bafer nije prazan*/
        if(empty2 < N2)
        {
            printf("Nit N2 je ocitala podatak %d\n", B2[head2]);
            head2 = (head2 + 1) % N2;
            empty2++;
            
            /*Obavestava N0 da se pojavila slobodna pozicija*/
            sem_post(&mutex_empty);
        }
        sem_post(&mutex_B2);
        
        /*Pravi pauzu pre citanja sledeceg podatka*/
        sleep(rand() % 7);
    }
}

void* nit3(void* arg)
{
    while(1)
    {
        /*Baferu se pristupa u kriticnoj sekciji*/
        sem_wait(&mutex_B3);
        
        /*Podatak se cita samo ukoliko bafer nije prazan*/
        if(empty3 < N3)
        {
            printf("Nit N3 je ocitala podatak %d\n", B3[head3]);
            head3 = (head3 + 1) % N3;
            empty3++;
            
            /*Obavestava N0 da se pojavila slobodna pozicija*/
            sem_post(&mutex_empty);
        }
        sem_post(&mutex_B3);
        
        /*Pravi pauzu pre citanja sledeceg podatka*/
        sleep(rand() % 9);
    }
}

int main()
{
    int i;
    
    /*Identifikatori niti*/
    pthread_t niti[4];
    
    srand(3232234);
    
    /*Kreiranje i inicijalizacija semafora*/
    sem_init(&mutex_B1, 0, 1);
    sem_init(&mutex_B2, 0, 1);
    sem_init(&mutex_B3, 0, 1);
    
    /*Semafor mutex_empty pamti ukupan broj slobodnih pozicija u sva 3 bafera*/
    
    sem_init(&mutex_empty, 0, N1 + N2 + N3);
    
    /*Kreiranje niti N0, N1, N2, N3*/
    pthread_create(&niti[1], NULL, (void*)nit1, (void*) NULL);
    pthread_create(&niti[2], NULL, (void*)nit2, (void*) NULL);
    pthread_create(&niti[3], NULL, (void*)nit3, (void*) NULL);
    pthread_create(&niti[0], NULL, (void*)nit0, (void*) NULL);
    
    /*Ceka se da se niti zavrse*/
    for(i=0; i<4; i++)
        pthread_join(niti[i], NULL);
    
    /*Brisanje semafora*/
    sem_destroy(&mutex_B1);
    sem_destroy(&mutex_B2);
    sem_destroy(&mutex_B3);
    sem_destroy(&mutex_empty);
}
