/*Linux program koji simulira problem proizvodjac/potrosac koriscenjem redova poruka(message-queries). Glavni program se deli u 2 procesa. Prvi proces(proizvodjac) kreira N slucajnih pozitivnih celih brojeva i salje ih drugom procesu. N se odredjuje tokom izvrsenja, takodje kao slucajan pozitivan ceo broj. Po zavrsetku slanja, prvi proces salje -1 kao kod za kraj. Drugi proces(potrosac) preuzima poslate brojeve iz poruka i salje ih na std izlazu*/

//Kod za proces dete
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>

#define RED_PORUKA 10001
#define MAX_PORUKA 10

struct poruka
{
    long tip;
    char tekst[MAX_PORUKA];
};

int main()
{
    int N; //broj poruka koje ce biti poslate
    int redid;
    int i;
    struct poruka bafer;
    
    /*Pribavlja se referenca na red poruka. Red se kreira ukoliko ne postiji*/
    redid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    if(redid < 0)
    {
        printf("Doslo je do greske prilikom preuzimanja reda poruka!\n");
        exit(1);
    }
    
    //Odredjuje se broj poruka koje ce biti poslate
    N = rand() % 20;
    
    //U red se salje N slucajnih pozitivnih celih brojeva i (-1) na kraju za kraj komunikacije
    for(i=0; i < N + 1; i++)
    {
        if(i == N)
        {
            sprintf(bafer.tekst, "%d", -1); //poslednja poruka je -1
        }
        else
        {
            sprintf(bafer.tekst, "%d", rand() % 100); //slucajno generisani brojevi
        }
        
        //Tip poruke u ovom primeru nije bitan i postavlja se na 1
        bafer.tip = 1;
        
        //Poruka se salje u red
        if(msgsnd(redid, &bafer, sizeof(bafer.tekst), 0) < -1)
        {
            printf("Doslo je do greske prilikom slanja poruke!\n");
            exit(1);
        }
    }
    return 0;
}
