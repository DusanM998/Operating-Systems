/*Linux program koji kao argumente komandne linije prihvata imena dve tekstualne datoteke. Program kreira dve niti.
 * Prva nit otvara datoteku cije je ime prvi argument komandne linije i cita liniju po liniju. Druga nit na isti nacin
 * otvara i cita liniju po liniju iz druge datoteke cije je ime drugi argument komandne linije. Ove dve niti naizmenicno
 * upisuju procitane linije u zajednicku izlaznu datoteku pod imenom "zbir.txt" tako da su neparne linije u zbirnoj datoteci
 * iz prve ulazne datoteke, a parne linije iz druge ulazne datoteke. Svaka nit ispred linije teksta u zbirnu datoteku treba da upise
 * ime izvorisne datoteke i broj linije iz izvorisne datoteke sa koje je procitana ta linija teksta*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 1024

FILE *output_file;
const char *file1;
const char *file2;
pthread_mutex_t mutex;
pthread_cond_t cond;
int line_number = 1;

void* processFile1(void *arg)
{
    FILE *input_file = fopen(file1, "r");
    if(input_file == NULL)
    {
        perror("Greska prilikom otvaranja prve datoteke!\n");
        return NULL;
    }
    
    char line[MAX_LINE_LENGTH];
    int line_count1 = 0;
    
    while(fgets(line, sizeof(line), input_file) != NULL)
    {
        pthread_mutex_lock(&mutex);
        
        while(line_number % 2 == 0)
            pthread_cond_wait(&cond, &mutex);
        
        line_count1++;
        
        fprintf(output_file, "%s: %d: %s", file1, line_count1, line);
        
        line_number++;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    fclose(input_file);
    
    return NULL;
}

void* processFile2(void *arg)
{
    FILE *input_file = fopen(file2, "r");
    if(input_file == NULL)
    {
        perror("Greska prilikom otvaranja druge datoteke!\n");
        return NULL;
    }
    
    char line[MAX_LINE_LENGTH];
    int line_count2 = 0;
    
    while(fgets(line, sizeof(line), input_file) != NULL)
    {
        pthread_mutex_lock(&mutex);
        
        while(line_number % 2 != 0)
            pthread_cond_wait(&cond, &mutex);
        
        line_count2++;
        
        fprintf(output_file, "%s: %d: %s", file2, line_count2, line);
        
        line_number++;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    fclose(input_file);
    
    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Neispravan unos! Unesite argumente: %s <prva_datoteka> <druga_datoteka>\n", argv[0]);
        return 1;
    }
    
    //Uzima prvi i drugi argument komandne linije
    file1 = argv[1];
    file2 = argv[2];
    
    output_file = fopen("zbir.txt", "w");
    if(output_file == NULL)
    {
        perror("Greska prilikom otvaranje izlazne datoteke!\n");
        return 1;
    }
    
    pthread_t nit1, nit2;
    
    pthread_create(&nit1, NULL, processFile1, NULL);
    pthread_create(&nit2, NULL, processFile2, NULL);
    
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    
    fclose(output_file);
    
    return 0;
}
