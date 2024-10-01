/*Linux program koji prvo startuje drugu nit, a zatim u glavnoj niti od korisnika ocekuje da unese ceo broj.
 * Drugoj niti prosledjuje uneti broj, a druga nit odbrojava(i stampa na ekranu brojeve kako odbrojava) od 0 do unetog broja
 * sa pauzom od 3 sekunde izmedju dva uzastopna broja. Zatim u glavnoj niti korisnik ponovo unosi ceo broj, prosledjuje ga drugoj niti
 * koja odbrojava i ovaj proces odbrojavanja i unosa broja se ponavlja sve dok korisnik ne unese broj 99.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int broj; //Globalna prom. za deljenje broja izmedju niti
pthread_mutex_t mutex; //Mutex za sinhronizaciju niti
pthread_cond_t cond; //Uslovna promenljiva za sinhronizaciju

//Funkcija koju izvodi druga nit
void* odbrojavanje(void* arg)
{
    while(1)
    {
        int i;
        pthread_mutex_lock(&mutex); //Zakljucavanje mutexa
        
        //Cekanje dok glavna nit ne unese broj
        pthread_cond_wait(&cond, &mutex);
        
        //Ako korisnik unese broj 99, nit se prekida
        if(broj == 99)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        //Odbrojavanje od 0 do unetog broja
        for(i=0; i <= broj; i++)
        {
            printf("Odbrojavanje: %d\n", i);
            
            if(i < broj)
                sleep(3);
        }
        
        printf("Odbrojavanje do %d zavrseno, unesite opet broj(99 je za izlaz): \n", i-1);
        
        pthread_mutex_unlock(&mutex); //Otkljucavanje mutexa
        
    }
    return NULL;
}

int main()
{
    pthread_t nit;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    //Startovanje druge niti
    pthread_create(&nit, NULL, odbrojavanje, NULL);
    
    while(1)
    {
        printf("Unesite ceo broj (99 je za izlaz): \n");
        scanf("%d", &broj);
        
        pthread_mutex_lock(&mutex); //Zakljucavanje mutexa
        pthread_cond_signal(&cond); //Signalizacija drugoj niti da je unet novi broj
        pthread_mutex_unlock(&mutex); //Otkljucavanje mutexa
        
        //Ako korisnik unese 99, program se prekida
        if(broj == 99)
        {
            printf("Odbrojavanje zavrseno, unet je broj 99!\n");
            break;
        }
    }
    
    //Ceka da se druga nit zavrsi
    pthread_join(nit, NULL);
    pthread_mutex_destroy(&mutex); //Unistavanje mutexa
    pthread_cond_destroy(&cond); //Unistavanje uslovne promenljive
    
    return 0;
}
