/*Linux program u kome pet niti pristupa deljivom celobrojnom nizu. Cetiri niti periodicno (na 3s) slucajno odabranom elementu niza dodaju generisanu celobrojnu vrednost
 * iz opsega [-10: 10]. Poslednja nit stampa elemente niza samo ukoliko je njihova suma (nakon izmene od strane ostalih niti) paran broj. Za sinhronizaciju koristiti
 * mehanizam mutex-a i uslovnih promenljivih.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define ARRAY_SIZE 10
#define BROJ_NITI 4

int niz[ARRAY_SIZE];
pthread_mutex_t mutex;
pthread_cond_t cond;
int suma = 0;

// Funkcija niti koja nasumično menja elemente niza
void *modify_array(void *arg)
{
    while(1)
    {
        int index = rand() % ARRAY_SIZE;
        int vrednost = (rand() % 21) - 10; // Generisanje vrednosti u opsegu od [-10: 10]

        pthread_mutex_lock(&mutex);
        
        niz[index] += vrednost;
        suma += vrednost; // Ažuriranje sume

        // Signalizacija niti koja proverava sumu
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
        
        sleep(3); // Pauza između izmena
    }

    return NULL;
}

// Funkcija niti koja proverava da li je suma parna i ispisuje niz
void *printf_if_sum_even(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);

        // Čekanje dok nit ne signalizira promenu
        pthread_cond_wait(&cond, &mutex);

        // Provera da li je suma parna
        if(suma % 2 == 0)
        {
            printf("Niz (parna suma): ");
            printf("Suma elemenata niza: %d\n", suma);
            for(int i = 0; i < ARRAY_SIZE; i++)
                printf("%d ", niz[i]);
            printf("\n");
        }

        pthread_mutex_unlock(&mutex);

        usleep(10000); // Pauza da se izbegne preopterećenje procesora
    }

    return NULL;
}

int main()
{
    pthread_t niti[BROJ_NITI + 1];

    srand(time(NULL)); // Inicijalizacija generatora slučajnih brojeva

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Inicijalizacija niza i sume
    for(int i = 0; i < ARRAY_SIZE; i++)
        niz[i] = 0;
    suma = 0;

    // Kreiranje niti za modifikovanje niza
    for(int i = 0; i < BROJ_NITI; i++)
        pthread_create(&niti[i], NULL, modify_array, NULL);

    // Kreiranje niti za proveru parnosti sume i ispis niza
    pthread_create(&niti[BROJ_NITI], NULL, printf_if_sum_even, NULL);

    // Čekanje na završetak svih niti (u ovom slučaju nikada se neće završiti jer su petlje beskonačne)
    for(int i = 0; i < BROJ_NITI + 1; i++)
        pthread_join(niti[i], NULL);

    // Oslobađanje resursa
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

