/*Linux program koji ucitava podatke iz txt datoteke cela( red po red) i zatim koriscenjem reda poruka sve parne redove salje procesu koji konvertuje sva slova u velika i zapisuje ih u datoteku pola1, a sve neparne redove procesu koji konvertuje sva slova u mala i zapisuje ih u datoteku pola2.*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>

#define RED_PORUKA 10301
#define MAX_PORUKA 255

struct poruka
{
    long tip;
    char tekst[MAX_PORUKA];
};

int main()
{
    int pid;
    int redid;
    struct poruka bafer;
    FILE* f;
    int poruka;
    char tmp[MAX_PORUKA];
    
    /*Pribavlja se referenca na red poruka. Red se kreira ukoliko ne postoji*/
    redid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    
    /*Kreiramo proces za parne linije*/
    pid = fork();
    
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(1);
    }
    
    if(pid == 0)
    {
        /*Izvrsava se proces dete*/
        if(execl("zad6a", "zad6a", "pola1", "1", NULL) < 0)
            printf("Doslo je do greske prilikom ucitavanja izvrsne datoteke!\n");
        exit(1);
    }
    
    /*Kreiramo proces za neparne linije*/
    pid = fork();
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(1);
    }
    if(pid == 0)
    {
        //Izvrsava proces dete
        if(execl("zad6a", "zad6a", "pola2", "2", NULL) < 0)
            printf("Doslo je do greske prulikom ucitavanja izvrsne datoteke!\n");
        exit(1);
    }
    
    //Kod koji izvrsava samo proces roditelj
    if(redid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja novog procesa!\n");
        exit(1);
    }
    
    f = fopen("cela", "r");
    
    if(f == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja datoteke!\n");
        exit(1);
    }
    
    //Cita se linija po linija iz datoteke
    poruka = 0;
    fgets(tmp, MAX_PORUKA, f);
    do
    {
        strcpy(bafer.tekst, tmp);
        
        if(poruka % 2 == 0)
            bafer.tip = 1; //parne poruke su tipa 1
        else
            bafer.tip = 2; //neparne poruke su tipa 2
                
        if(msgsnd(redid, &bafer, MAX_PORUKA, 0) < 0)
        {
            printf("Doslo je do greske prilikom slanja poruke!\n");
            exit(1);
        }
        poruka++;
        fgets(tmp, MAX_PORUKA, f);
    }
    while(!feof(f));
    
    fclose(f);
    
    /*Saljemo poruku sa sadrzajem 'END' za oba procesa*/
    strcpy(bafer.tekst, "END");
    
    bafer.tip = 1;
    msgsnd(redid, &bafer, MAX_PORUKA, 0);
    
    bafer.tip = 2;
    msgsnd(redid, &bafer, MAX_PORUKA, 0);
    
    //Red poruka se brise tek kada se zavrse oba deteta
    wait(NULL);
    wait(NULL);
    msgctl(redid, IPC_RMID, 0);
    
    return 0;
}
