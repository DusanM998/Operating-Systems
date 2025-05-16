/*Na standardnom izlazu ispisuje recenicu "Ovo je test za niti!", pri cemu svaku rec ispisuje posebna nit */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

/*Niz koji sadrzi reci koje se stampaju deklarisemo kao globalnu promenljivu kako bi bile vidljive u svim f-jama pa i u f-ji niti*/
char reci[N][20] = {"Ovo ", "je ", "test ", "za ", "niti!"};

/*Struktura koja definise ulazne argumente niti(zato sto pthread_create prihvata samo jedan pointer na argument). Definisemo je da bismo mogli da istampamo 2 stvari*/
struct argument
{
    /*Redni broj reci koja se stampa*/
    int rec;
    /*Nit koja treba da odstampa prethodnu rec i koja se zbog toga ceka*/
    pthread_t prethodna_nit;
};

/*F-ja niti*/
void * stampanje(void * arg)
{
    /*Ulazni argument je struktura koja sadrzi neophodne informacije za stampanje*/
    struct argument * p = (struct argument *)arg;
    
    /*samo nit koja stampa 1. rec ne ceka prethodnu nit, dok sve ostale niti cekaju nit koja stampa prethodnu rec*/
    if(p->rec > 0) //1.
        pthread_join(p->prethodna_nit, NULL);//2. (da nemamo 1. i 2. vrednosti bi bile random ispisane)
    printf("%s", reci[p->rec]);
    fflush(stdout);
    sleep(3);
}

int main()
{
    int i;
    /*Niz ulaznih argumenata niti. Po 1 za svaku nit*/
    struct argument args[N];
    
    /*Niz identifikatora niti. Po 1 za svaku rec*/
    pthread_t niti[N];
    
    /*Kreiranje niti*/
    for(i=0; i < N; i++)
    {
        args[i].rec = i;
        if(i > 0)
            args[i].prethodna_nit = niti[i-1];
        pthread_create(&niti[i], NULL, (void*) stampanje, (void*)&args[i]);
        
    }
        /*Main f-ja ceka da se poslednja nit zavrsi*/
        pthread_join(niti[N-1], NULL);
        
        return 0;
}
