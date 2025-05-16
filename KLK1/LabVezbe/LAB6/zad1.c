/*Linux program koji onemogucava da se tekuci proces prekine jednim pritiskom kombinacijom tastera Ctrl+C. Proces se prekida tek kada se ova kombinacija tastera pritisne 2 puta za redom.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/*F-ja za obradu signala*/
void catch_int(int sig_sum)
{
    /*Postavlja se default f-ja za obradu signala za sledeci put kada bude pritisnuta kombinacija Ctrl+C*/
    signal(SIGINT, SIG_DFL);
    printf("Nije moguce prekinuti program!\n");
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    /*Postavlja f-ju catch_int da obradjuje signal SIGINT*/
    signal(SIGINT, catch_int);
    
    /*Beskonacna petlja*/
    for( ; ;)
        pause();
}
