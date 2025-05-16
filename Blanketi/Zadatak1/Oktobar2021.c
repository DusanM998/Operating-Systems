/*Linux program u kome korisnik sa tastature unosi recenicu. Glavni program zatim kreira dve niti.
 * Prva nit sortira reci u leksikografski rastucem redosledu, a po zavrsetku sortiranja druga nit stampa
 * na ekranu tu recenicu tako da je pocetno slovo u odstampanoj recenici veliko, a da se recenica zavrsava tackom.
 * Po zavrsetku rada ove dve niti, glavni program trebda da odstampa KRAJ.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_LENGTH 1024
#define MAX_WORDS 100

char recenica[MAX_LENGTH]; // Originalna rečenica
char sortirana_recenica[MAX_LENGTH]; // Sortirana rečenica
int brojReci;
char *reci[MAX_WORDS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex za sinhronizaciju niti
pthread_cond_t cond_sortirano = PTHREAD_COND_INITIALIZER; // Uslovna promenljiva za signalizaciju sortiranja
int sortirano = 0; // Signal da je sortiranje završeno

// Funkcija za sortiranje reči (prva nit)
void *sortirajReci(void *arg)
{
    pthread_mutex_lock(&mutex);
    
    // Kopiranje rečenice za tokenizaciju
    char temp_recenica[MAX_LENGTH];
    strcpy(temp_recenica, recenica);
    
    // Razdvajanje reči
    char *token = strtok(temp_recenica, " ");
    brojReci = 0;
    while(token != NULL && brojReci < MAX_WORDS)
    {
        reci[brojReci] = strdup(token);
        if(reci[brojReci] == NULL)
        {
            perror("Greska pri alokaciji memorije sa strdup\n");
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        brojReci++;
        token = strtok(NULL, " ");
    }
    
    // Sortiranje reči u leksikografski rastućem redosledu
    for(int i = 0; i < brojReci - 1; i++)
    {
        for(int j = i + 1; j < brojReci; j++)
        {
            if(strcmp(reci[i], reci[j]) > 0)
            {
                char *tmp = reci[i];
                reci[i] = reci[j];
                reci[j] = tmp;
            }
        }
    }
    
    // Kreiranje sortirane rečenice
    if(brojReci > 0)
    {
        strcpy(sortirana_recenica, reci[0]);
    }
    for(int i = 1; i < brojReci; i++)
    {
        if(strlen(sortirana_recenica) + strlen(reci[i]) + 1 < MAX_LENGTH)
        {
            strcat(sortirana_recenica, " ");
            strcat(sortirana_recenica, reci[i]);
        }
        else
        {
            fprintf(stderr, "Greska! Recenica je predugacka.\n");
            break;
        }
    }
    
    // Oslobađanje memorije
    for(int i = 0; i < brojReci; i++)
        free(reci[i]);
    
    sortirano = 1; // Signalizacija da je sortiranje završeno
    pthread_cond_signal(&cond_sortirano); // Signalizacija drugoj niti
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

// Funkcija za štampanje sortirane rečenice (druga nit)
void *formirajRecenicu(void *arg)
{
    pthread_mutex_lock(&mutex);
    
    // Čekanje dok prva nit ne završi sortiranje
    while(!sortirano)
    {
        pthread_cond_wait(&cond_sortirano, &mutex);
    }
    
    // Pretvaranje prvog slova u veliko slovo i dodavanje tačke
    if(strlen(sortirana_recenica) > 0)
    {
        sortirana_recenica[0] = toupper(sortirana_recenica[0]);
        if(strlen(sortirana_recenica) + 1 < MAX_LENGTH)
        {    
            strcat(sortirana_recenica, ".");
        }
        else
        {
            fprintf(stderr, "Greska! Recenica je predugacka.\n");
        }
    }
    
    // Ispisivanje rečenice
    printf("Sortirana recenica: %s\n", sortirana_recenica);
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main()
{
    pthread_t nit1, nit2;
    
    // Učitavanje rečenice od strane korisnika
    printf("Unesite recenicu:\n");
    fgets(recenica, MAX_LENGTH, stdin);
    // Uklanjanje novog reda sa kraja
    recenica[strcspn(recenica, "\n")] = '\0';
    
    // Kreiranje niti za sortiranje reči
    pthread_create(&nit1, NULL, sortirajReci, NULL);
    
    // Kreiranje niti za formiranje rečenice
    pthread_create(&nit2, NULL, formirajRecenicu, NULL);
    
    // Čekanje da se obe niti završe
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    
    printf("KRAJ\n");
    
    return 0;
}

