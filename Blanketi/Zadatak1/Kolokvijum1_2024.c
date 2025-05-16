/*Linux program koji kreira dve dodatne niti. U glavni program se preko argumenata komandne linije unose nazivi tri tekstualne datoteke.
 * Prvoj niti se prosledjuje naziv tekstualne datoteke koji je bio prvi argument komandne linije, a drugoj niti se prosledjuje naziv
 * tekstualne datoteke koji je bio drugi argument komandne linije. Obe niti naizmenicno u tekstualnu datoteku cije je ime uneto kao treci
 * argument komandne linije upisuju linije teksta iz tekstualne ciji je naziv prosledjen toj niti i to tako sto nit prvo upisuje naziv tekstualne
 * datoteke iz koje je linije teksta, zatim upisuje znak ":" pa onda samu liniju teksta. Upisivanje krece sa linijom teksta iz druge tekstualne datoteke
 * i na dalje se linije teksta naizmenicno smenjuju iz prve i druge tekstualne datoteke.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_LEN 1024

FILE* outputFile; 
char *file1, *file2;

pthread_mutex_t mutex;
pthread_cond_t cond;

int prvaRed = 0;

void *processFile1(void *args)
{
    FILE* f1 = fopen(file1,"r");
    if(f1 == NULL)
    {
        printf("Greska prilikom otvaranja fajla! Fajl ne postoji na disku!\n");
        return NULL;
    }
    
    char line[MAX_LEN];
    int line_count1 = 0;
    
    while(fgets(line,sizeof(line), f1))
    {
        pthread_mutex_lock(&mutex);
        while(prvaRed == 0)
            pthread_cond_wait(&cond, &mutex);
        
        line_count1++;
        
        fprintf(outputFile, "%s: %d : %s", file1, line_count1, line);
        
        prvaRed = 0;
        
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *processFile2(void *args)
{
    FILE* f2 = fopen(file2,"r");
    if(f2 == NULL)
    {
        printf("Greska prilikom otvaranja fajla! Fajl ne postoji na disku!\n");
        return NULL;
    }
    
    char line[MAX_LEN];
    int line_count2 = 0;
    
    while(fgets(line,sizeof(line), f2))
    {
        pthread_mutex_lock(&mutex);
        while(prvaRed == 1)
            pthread_cond_wait(&cond, &mutex);
        
        line_count2++;
        
        fprintf(outputFile, "%s: %d : %s", file2, line_count2, line);
        
        prvaRed = 1;
        
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printf("Nedovoljno argumenata za %s. Uneti <file1, file2, outputFile>\n", argv[0]);
        exit(1);
    }
    
    file1 = argv[1];
    file2 = argv[2];
    outputFile = fopen(argv[3], "w");
    
    if(outputFile == NULL)
    {
        printf("Greska prilikom otvaranja oredisnog fajla!\n");
        exit(1);
    }
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    pthread_t nit1, nit2;
    
    pthread_create(&nit1, NULL, processFile1, NULL);
    pthread_create(&nit2, NULL, processFile2, NULL);
    
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    fclose(outputFile);
    
    return 0;
}
