/*Linux program koji u glavnoj niti od korisnika ocekuje da unese ceo broj. Zatim startuje drugu nit, prosledjuje joj uneti broj,
 * a druga nit odbrojava(i stampa na ekranu brojeve kako odbrojava) od unetog broja do 0 sa pauzom od 1 sekunde izmedju dva uzastopna broja.
 * Zatim u glavnoj niti korisnik ponovo unosi ceo broj i ovaj proces odbrojavanja i unosa broja se ponavlja sve dok korisnik ne unese "KRAJ".*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int broj; //Globalna prom. za deljenje broja izmedju niti
pthread_mutex_t mutex; //Mutex za sinhronizaciju niti
pthread_cond_t cond; //Uslovna promenljiva za sinhronizaciju
int kraj = 0; //Promenljiva koja signalizira kraj programa

//Funkcija koju izvodi druga nit
void* odbrojavanje(void* arg)
{
    while(1)
    {
        int i;
        pthread_mutex_lock(&mutex); //Zakljucavanje mutexa
        
        //Cekanje dok glavna nit ne unese broj
        pthread_cond_wait(&cond, &mutex);
        
        //Ako korisnik unese KRAJ prekida se program
        if(kraj)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        //Odbrojavanje od 0 do unetog broja
        for(i=0; i <= broj; i++)
        {
            printf("Odbrojavanje: %d\n", i);
            sleep(1);
        }
        
        printf("Odbrojavanje do %d zavrseno, unesite opet broj: \n", i-1);
        
        pthread_mutex_unlock(&mutex); //Otkljucavanje mutexa
        
    }
    return NULL;
}

int main()
{
    pthread_t nit;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    char input[10]; //Promenljiva za unos
    
    //Startovanje druge niti
    pthread_create(&nit, NULL, odbrojavanje, NULL);
    
    while(1)
    {
        printf("Unesite ceo broj ili KRAJ za izlaz iz programa: \n");
        scanf("%s", input);
        
        pthread_mutex_lock(&mutex); //Zakljucavanje mutexa
        
        //Provera da li je korisnik uneo KRAJ
        if(strcmp(input, "KRAJ") == 0)
        {
            kraj = 1;
            pthread_cond_signal(&cond); //Signalizacija drugoj niti da je unet KRAJ
            pthread_mutex_unlock(&mutex); //Otkljucavanje mutexa
            printf("Unet je KRAJ!\n");
            break;
        }
        else
        {
            //Ako nije uneta rec KRAJ, unosi se broj
            broj = atoi(input); //Konverzija inputa u broj, jer je input string
            pthread_cond_signal(&cond); //Signalizacija drugoj niti da je unet broj
        }
        
        pthread_mutex_unlock(&mutex); //Otkljucavanje mutexa
    }
    
    //Ceka da se druga nit zavrsi
    pthread_join(nit, NULL);
    pthread_mutex_destroy(&mutex); //Unistavanje mutexa
    pthread_cond_destroy(&cond); //Unistavanje uslovne promenljive
    
    return 0;
}
