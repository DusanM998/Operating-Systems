#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 4

/*Matrica za sortiranje je deklarisana kao globalna promenljiva da bi bila vidljiva u svim funkcijama pa samim tim i u funkciji niti*/
int a[N] [N];

/*Funkcija niti*/
void * sortiranje(void * arg)
{
    int k = *((int *)arg);
    int i, j, pom;
    /*sortiranje matrice*/
    printf("Sortiram vrstu: %d\n", k);
    for(i=0; i < N - 1; i++)
        for(j=i+1; j < N; j++)
            if(a[k][i] > a[k][j])
            {
                pom = a[k][i];
                a[k][i] = a[k][j];
                a[k][j] = pom;
            }
    free(arg);
}

int main()
{
    int i,j, *row; 
    /*Niz identifikatora niti. Po 1 za svaku vrstu*/
    pthread_t niti[N];
    /*Sa standardnog ulaza citamo elemente matrice*/
    for(i=0; i < N; i++)
        for(j=0; j < N; j++)
            scanf("%d", &a[i][j]);
    for(i=0; i < N; i++)
    {
        for(j=0; j < N; j++)
            printf("%d\t", a[i][j]);
        printf("\n");
    }
    printf("\n");
    /*Kreiramo niti */
    for(i=0; i < N; i++)
    {
        row = (int*) malloc(sizeof(int)); //pre nego sto se na izvrsenje poziva thread kreira se argument u dinamickoj zoni memoriji
        *row = i;
        pthread_create(&niti[i], NULL, (void*)sortiranje, (void*)row);
    }
    /*Main f-ja ceka da se niti zavrse*/
    for(i=0; i < N; i++)
        pthread_join(niti[i], NULL);
    /*Na standardnom izlazu stampamo sortiranu matricu*/
    for(i=0; i < N; i++)
    {
        for(j=0; j < N; j++)
            printf("%d\t", a[i][j]);
        printf("\n");
    }
    return 0;
}
