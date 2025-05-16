/*Linux program koji kao argumente komandne linije prihvata 2 txt datoteke. Program kreira 2 niti. Prva nit otvara datoteku cije je ime prvi argument komandne linije i cita liniju po liniju. Druga nit na isti nacin otvara i cita liniju po liniju iz datoteke cije je ime drugi argument komandne linije. Ove dve niti naizmenicno upisuju procitane linije u zajednicku izlaznu datoteku pod imenom zbir.txt tako da su neparne linije u zbirnoj datoteci iz prve ulazne datoteke, a parne linije iz druge ulazne datoteke. Svaka nit ispred linije teksta u zbirnu datoteku treba da upise ime izvorisne datoteke i broj linije iz izvorisne datoteke sa koje je procitana ta linija teksta*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

int prvaFinished = 0;
int drugaFinished = 0;
int prvaJustFinished = 0;
int drugaJustFinished = 0;

sem_t mutex1;
sem_t mutex2;

void* prvaNit(void* arg)
{
    FILE* source;
    FILE* destination;
    char red[1000];
    int brReda = 1;
    char* datoteka = (char*)arg;
    char output[1500] = "";
    
    source = fopen(datoteka, "r");
    
    fgets(red, 1000, source);
    
    while(!feof(source))
    {
        if(drugaFinished == 0)
            sem_wait(&mutex1);
        
        destination = fopen("./zbir.txt", "a");
        
        if(drugaFinished == 1 && drugaJustFinished == 0)
            fputs("\n", destination);
        if(drugaFinished == 1 && drugaJustFinished == 1)
            drugaJustFinished = 0;
        
        strcat(output, "Prva ");
        strcat(output, datoteka);
        strcat(output, ":");
        char rbr[10];
        sprintf(rbr, "%d", brReda);
        strcat(output, rbr);
        strcat(output, ":");
        strcat(output, red);
        
        fputs(output, destination);
        
        brReda++;
        
        fclose(destination);
        
        sem_post(&mutex2);
        
        fgets(red, 1000, source);
        strcpy(output, "");
    }
    prvaFinished = 1;
    prvaJustFinished = 1;
    fclose(source);
}

void* drugaNit(void* arg)
{
    FILE* source;
    FILE* destination;
    char red[1000];
    int brReda = 1;
    char* datoteka = (char*)arg;
    char output[1500];
    
    source = fopen(datoteka, "r");
    
    fgets(red, 1000, source);
    
    while(!feof(source))
    {
        if(prvaFinished == 0)
            sem_wait(&mutex2);
        
        destination = fopen("./zbir.txt", "a");
        
        if(prvaFinished == 1 && prvaJustFinished == 0)
            fputs("\n", destination);
        if(prvaFinished == 1 && prvaJustFinished ==1)
            prvaJustFinished = 0;
        
        strcat(output, "Druga ");
        strcat(output, datoteka);
        strcat(output, ":");
        char rbr[10];
        sprintf(rbr, "%d", brReda);
        strcat(output, rbr);
        strcat(output, ":");
        strcat(output, red);
        
        fputs(output, destination);
        
        brReda++;
        
        fclose(destination);
        sem_post(&mutex1);
        
        fgets(red, 1000, source);
        strcpy(output, "");
    }
    drugaFinished = 1;
    drugaJustFinished = 1;
    fclose(source);
}

int main(int argc, char* argv[])
{
    pthread_t nit1;
    pthread_t nit2;
    
    if(argc != 3)
    {
        printf("Pogresni ulazni parametri!\n");
        exit(-1);
    }
    
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 0);
    
    pthread_create(&nit1, NULL, (void*)prvaNit, argv[1]);
    pthread_create(&nit2, NULL, (void*)drugaNit, argv[2]);
    
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    
    exit(0);
}
