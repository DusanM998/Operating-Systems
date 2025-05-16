/*Linux program koji od korisnika ocekuje da sa tastature unese korisnicko ime. Ukoliko korisnik ne unese nista u roku od 30 sekundi, proces obavestava korisnika da je vreme za unos isteklo*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

char user[40];

/*Handler za alarm*/
void catch_alarm(int sig_sum)
{
    printf("\nOperacija je istekla. Izlazim...\n\n");
    exit(0);
}

int main(int argc, char* argv[])
{
    /*Signal handler za ALARM signal*/
    signal(SIGALRM, catch_alarm);
    
    /*Prompt korisniku da unese string*/
    printf("Username: ");
    fflush(stdout);
    
    /*Startuje alarm od 30 sekundi*/
    alarm(30);
    
    /*Ceka da korisnik unese ime*/
    gets(user);
    
    /*Ukoliko korisnik unese ime, uklanja alarm*/
    alarm(0);
    
    printf("Username: '%s'\n", user);
    return 0;
}
