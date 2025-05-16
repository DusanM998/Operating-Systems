/*Pristup bazi pod. obavlja se radi upisa i citanja od strane vise procesa. U jednom trenutku moze postojati vise procesa koji citaju sadrzaj iz BP proce procedurom read_database(), ali ako jedan proces upisuje sadrzaj u bazu pod. procedurom write_database(), nijednom drugom procesu nije dozvoljen pristup bazi pod. radi upisa i citanja. Prednost imaju procesi koji citaju sadrzaj, tako da dok god ima procesa koji citaju iz BP, proces koji treba da upisuje pod. mora da ceka. Koriscenjem programskog jezika C napisati Linux program koji koriscenjem procesa i poznatih IPC mehanizama simulira prethodno opisani algoritam(Sinhronizacioni problem: Citaoci - pisci) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int A = 0; //Globalna prom. koja predstavlja BP
int N = 0; //Globalna prom. koja predstavlja brojac citaoca koji u datom trenutku koriste BP

sem_t mutex_DB, mutex_N; //semafori za medjusobno ukljucenje

void read_database(int i)
{
    /*Operacija citanja iz BP se simulira stampanjem vrednosti globalne prom. A*/
    printf("Nit %d je procitala vrednost %d\n", i, A);
    sleep(rand() % 3);
}

void write_database()
{
    /*Operacija upisa u BP se simulira izenom vrednosti globalne prom. A*/
    printf("Pisac menja vrednost u BP\n");
    A = A + rand() % 10;
    sleep(rand() % 5);
}


void* pisac(void* arg)
{
    while(1)
    {
        /*Pristup bazi je smesten u kriticnu sekciju*/
        sem_wait(&mutex_DB);
        write_database();
        sem_post(&mutex_DB);
        
        /*Pravi pauzu pre sledeceg pristupa bazi podataka*/
        sleep(rand() % 10);
    }
}

void* citalac(void* arg)
{
    int i;
    i = *((int*)arg);
    
    while(1)
    {
        /*Promenljivoj N se pristupa u KS*/
        sem_wait(&mutex_N);
        
        /*Povecava se broj citalaca*/
        N++;
        
        /*Prvi citalac zakljucava bazu podataka kako bi sprecio pisca da joj pristupa*/
        if(N == 1)
            sem_wait(&mutex_DB);
        
        sem_post(&mutex_N);
        
        read_database(i);
        
        /*Promenljivoj N se pristupa u KS*/
        sem_wait(&mutex_N);
        
        /*Smanjuje se broj citalaca*/
        N--;
        
        /*Poslednji citalac otkljucava BP*/
        if(N == 0)
            sem_post(&mutex_DB);
        sem_post(&mutex_N);
        
        /*Pravi pauzu pre sledeceg pristupa bazi*/
        sleep(rand() % 7);
    }
}

int main()
{
    int i;
    int red_br[4];
    
    /*Identifikatori niti, 4 citaoca + 1 pisac*/
    pthread_t niti[5];
    
    srand(3232234);
    
    /*Kreiranje i inicijalizacija semafora*/
    sem_init(&mutex_N, 0, 1);
    sem_init(&mutex_DB, 0, 1);
    
    /*Kreiranje citalaca i pisca*/
    for(i=0; i < 4; i++)
    {
        red_br[i] = i;
        pthread_create(&niti[i], NULL, (void*)citalac, (void*)&red_br[i]);
    }
    pthread_create(&niti[4], NULL, (void*)pisac, (void*) NULL);
    
    /*Ceka se da se niti zavrse*/
    for(i=0; i < 5; i++)
        pthread_join(niti[i], NULL);
    
    /*Brisanje semafora*/
    sem_destroy(&mutex_N);
    sem_destroy(&mutex_DB);
}
