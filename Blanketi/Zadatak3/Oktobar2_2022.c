/*Koriscenjem prog. jez. C kreirati tri Linux procesa koji komuniciraju koriscenjem reda poruka. Prvi proces cita prvo cifru pa string sa tastature. 
 * Ukoliko je procitana cifra jednaka 1, koriscenjem redova poruka procitani string salje drugom procesu, a ukoliko je uneta cifra jednaka 2,
 * poruku salje trecem procesu. Drugi proces ispisuje svaku primljenu poruku i njen broj karaktera.
 * Treci proces primljenu poruku pretvara u velika slova, a zatim je ispisuje. Komunikacija se prekida i procesi izlaze kada korisnik unese cifru razlicitu od
 * 1 ili 2.*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>

#define MSGLEN 50
#define MAX_PORUKA 255
#define RED_PORUKA 10301

struct mymsgbuf 
{
    long mtype;
    char mtext[MSGLEN];
};

int main()
{
    int pid1, pid2;
    int msqid;
    struct mymsgbuf bafer;
    int broj;
    char poruka[MAX_PORUKA];
    
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    
    if(msqid == -1)
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    //Kreiranje drugog procesa: Ispisuje svaku primljenu poruku i njen broj karaktera
    pid1 = fork();
    if(pid1 < 0)
    {
        printf("Doslo je do greske prilikom kreiranja prvog procesa!\n");
        exit(1);
    }
    
    if(pid1 == 0)
    {
        while(1)
        {
            if(msgrcv(msqid, &bafer, MAX_PORUKA, 1, 0) == -1)
            {
                printf("Greska prilikom prijema poruke!\n");
                exit(1);
            }
            
            if(strcmp(bafer.mtext, "END") == 0)
                break;
            printf("Proces 2 primio poruku: %s\n", bafer.mtext);
            printf("Proces 2: Broj karaktera u poruci: %ld\n", strlen(bafer.mtext));
        }
        exit(0);
    }
    
    //Kreiranje treceg procesa: pretvara primljenu poruku u velika slova
    pid2 = fork();
    if(pid2 < 0)
    {
        printf("Doslo je do greske prilikom kreiranja drugog procesa!\n");
        exit(1);
    }
    
    if(pid2 == 0)
    {
        while(1)
        {
            if(msgrcv(msqid, &bafer, MAX_PORUKA, 2, 0) == -1)
            {
                printf("Greska prilikom prijema poruke!\n");
                exit(1);
            }
            
            if(strcmp(bafer.mtext, "END") == 0)
                break;
            
            //Konvertuje primljenu poruku u velika slova
            for(int i=0; i < bafer.mtext[i]; i++)
                bafer.mtext[i] = toupper(bafer.mtext[i]);
            
            printf("Proces 3: Primio sam poruku: %s i konvertovao je u velika slova\n", bafer.mtext);
        }
        exit(0);
    }
    
    //Proces roditelj(1. proces): sa tastature unosi podatke i salje ih preostala dva procesa
    while(1)
    {
        printf("Unesite broj (1 ili 2) i poruku: ");
        scanf("%d", &broj);
        getchar();
        fgets(poruka, MAX_PORUKA, stdin);
        poruka[strcspn(poruka, "\n")] = 0;
        
        //Ako je uneta cifra != od 1 ili 2, prekida se komunikacija
        if(broj != 1 && broj != 2)
        {
            strcpy(bafer.mtext, "END");
            
            bafer.mtype = 1;
            if(msgsnd(msqid, &bafer, strlen(bafer.mtext) + 1, 0) == -1)
            {
                printf("Greska prilikom slanja poruke!\n");
                exit(1);
            }
            
            bafer.mtype = 2;
            if(msgsnd(msqid, &bafer, strlen(bafer.mtext) + 1, 0) == -1)
            {
                printf("Gresla prilikom slanja poruke!\n");
                exit(1);
            }
            
            printf("Proces 1. (Roditelj): Primio sam cifru razlicitu od 1 i 2. Kraj komunikacije...\n");
            break;
        }
        
        //Slanje poruke u zavisnosti od unete cifre
        strcpy(bafer.mtext, poruka);
        bafer.mtype = broj;
        msgsnd(msqid, &bafer, strlen(bafer.mtext) + 1, 0);
    }
    
    wait(NULL);
    wait(NULL);
    
    msgctl(msqid, IPC_RMID, NULL);
    
    return 0;
}
