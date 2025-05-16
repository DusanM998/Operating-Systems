/*Linux program koji iz text. datoteke "ulaz.txt" ucitava nizove celih brojeva jedan po jedan (svaki niz u zasebnoj liniji, 
 * pri cemu su brojevi u liniji razdvojeni blanko znacima) u niz u memoriji. Glavni program odmah po startovanju kreira dodatnu nit
 * koja sumira brojeve ucitane u niz u memoriji. Po zavrsetku sumiranja, glavni program stampa sumu i prelazi na naredni ucitani niz.
 * Postupak se ponavlja sve dok se ne procitaju i sumiraju svi nizovi iz datoteke.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int *array = NULL;    // Dinamički alocirani niz za brojeve
int N = 0;
int sum = 0;
int done = 0;         // Signalizira kraj rada
pthread_mutex_t mutex;
pthread_cond_t cond;
int ready_to_sum = 0; // Signalizira da je niz spreman za sumiranje

void* sumirajNiz(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);

        while (!ready_to_sum && !done)
            pthread_cond_wait(&cond, &mutex);

        if (done) { // Ako je glavni program signalizirao kraj, nit se završava
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Sumiranje niza
        sum = 0;
        for (int i = 0; i < N; i++)
            sum += array[i];

        ready_to_sum = 0; // Reset statusa spremnosti za sledeći niz

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void process_line(char* line)
{
    // Parsira liniju i učitava brojeve u niz
    char* token = strtok(line, " ");
    N = 0;

    // Dinamička alokacija memorije za niz
    int *temp_array = malloc(MAX_LINE_LENGTH / 2 * sizeof(int));
    if (!temp_array) {
        perror("Neuspešna alokacija memorije!\n");
        exit(1);
    }

    while (token != NULL)
    {
        temp_array[N++] = atoi(token);
        token = strtok(NULL, " ");
    }

    // Premesti podatke u glavni niz i oslobodi prethodni
    pthread_mutex_lock(&mutex);
    free(array);
    array = temp_array;

    // Signalizuje da je niz spreman za sumiranje
    ready_to_sum = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    // Čeka da nit završi sumiranje
    pthread_mutex_lock(&mutex);
    while (ready_to_sum)
        pthread_cond_wait(&cond, &mutex);

    pthread_mutex_unlock(&mutex);

    // Ispis rezultata
    printf("Suma brojeva ucitanih iz fajla: %d\n", sum);
}

int main()
{
    FILE* file = fopen("ulaz.txt", "r");
    if (!file)
    {
        perror("Ne može se otvoriti fajl!\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    pthread_t sum_thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Kreira nit za sumiranje
    pthread_create(&sum_thread, NULL, sumirajNiz, NULL);

    // Obradjuje svaku liniju u datoteci
    while (fgets(line, sizeof(line), file)) {
        process_line(line);
    }

    // Signalizira niti da je obrada završena
    pthread_mutex_lock(&mutex);
    done = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    // Cekanje da se nit završi
    pthread_join(sum_thread, NULL);

    // Oslobađa resurse
    free(array);
    fclose(file);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("Sumirani svi nizovi iz datoteke. KRAJ.\n");

    return 0;
}
