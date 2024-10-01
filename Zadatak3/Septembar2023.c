/*Linux program koji ucitava podatke iz tekst. datoteke "poruka.txt" (red po red) i zatim koriscenjem jednog reda poruka
 * sve parne redove salje procesu koji konvertuje sva slova u velika, a sve neprne redove salje procesu koji konvertuje
 * sva slova u mala. Drugi i treci proces konvertovane linije upisuju u datoteku "sredjeno.txt".*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/file.h>

#define RED_PORUKA 10301
#define MAX_PORUKA 255

struct poruka 
{
    long tip;
    char tekst[MAX_PORUKA];
};

void konvertuj_u_mala(char *str)
{
    while(*str)
    {
        *str = tolower(*str);
        str++;
    }
}

void konvertuj_u_velika(char *str)
{
    while(*str)
    {
        *str = toupper(*str);
        str++;
    }
}

int main()
{
    int pid;
    int redid;
    struct poruka bafer;
    FILE *fileCitanje;
    int linija;
    char tmp[MAX_PORUKA];
    
    redid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    if(redid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    
    pid = fork();
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja prvog procesa!\n");
        exit(1);
    }
    
    if(pid == 0) //Proces 1: konvertuje mala slova u velika
    {
        FILE *fileUpis = fopen("sredjeno.txt", "a");
        if (fileUpis == NULL) 
        {
            printf("Doslo je do greske prilikom otvaranja datoteke za upis!\n");
            exit(1);
        }
        
        while(1)
        {
            msgrcv(redid, &bafer, MAX_PORUKA, 1, 0);
            if(strcmp(bafer.tekst, "END") ==  0)
                break;
            konvertuj_u_velika(bafer.tekst);
            printf("Proces 1 (velika slova): %s\n", bafer.tekst);
            
            // Zaključavanje fajla za siguran upis
            int fd = fileno(fileUpis);
            flock(fd, LOCK_EX);
            fprintf(fileUpis, "%s", bafer.tekst);
            fflush(fileUpis);
            flock(fd, LOCK_UN);  // Otključavanje fajla
        }
        
        fclose(fileUpis);
        
        exit(0);
    }
    
    pid = fork();
    if(pid < 0)
    {
        printf("Doslo je do greske prilikom kreiranja drugog procesa!\n");
        exit(1);
    }
    
    if(pid == 0) //Proces 2: konvertuje velika slova u mala
    {
        FILE *fileUpis = fopen("sredjeno.txt", "a");
        if (fileUpis == NULL) 
        {
            printf("Doslo je do greske prilikom otvaranja datoteke za upis!\n");
            exit(1);
        }
        
        while(1)
        {
            msgrcv(redid, &bafer, MAX_PORUKA, 2, 0);
            if(strcmp(bafer.tekst, "END") ==  0)
                break;
            konvertuj_u_mala(bafer.tekst);
            printf("Proces 2 (mala slova): %s\n", bafer.tekst);
            
            // Zaključavanje fajla za siguran upis
            int fd = fileno(fileUpis);
            flock(fd, LOCK_EX);
            fprintf(fileUpis, "%s", bafer.tekst);
            fflush(fileUpis);
            flock(fd, LOCK_UN);  // Otključavanje fajla
        }
        
        fclose(fileUpis);
        
        exit(0);
    }
    
    //Proces roditelj: cita podatke iz fajla "poruka.txt"
    fileCitanje = fopen("poruka.txt", "r");
    
    if(fileCitanje == NULL)
    {
        printf("Doslo je do greske prilikom otvaranja datoteke!\n");
        exit(1);
    }
    
    //Cita se linija po linija iz datoteke
    linija = 0;
    fgets(tmp, MAX_PORUKA, fileCitanje);
    do
    {
        strcpy(bafer.tekst, tmp);
        if(linija % 2 == 0)
            bafer.tip = 1; //Parni redovi imaju tip 1
        else
            bafer.tip = 2;
        
        if(msgsnd(redid, &bafer, strlen(bafer.tekst) + 1, 0) < 0)
        {
            printf("Doslo je do greske prilikom slanja poruke!\n");
            exit(1);
        }
        linija++;
        fgets(tmp, MAX_PORUKA, fileCitanje);
    }
    while(!feof(fileCitanje));
    fclose(fileCitanje);
    
    //Saljemo poruku sa sadrzajem END za oba procesa
    strcpy(bafer.tekst, "END");
    
    //Slanje zavrsnih poruka
    bafer.tip = 1;
    msgsnd(redid, &bafer, MAX_PORUKA, 0);
    
    bafer.tip = 2;
    msgsnd(redid, &bafer, MAX_PORUKA, 0);
    
    //Cekanje da se oba procesa zavrse
    wait(NULL);
    wait(NULL);
    
    //Brisanje reda poruka
    msgctl(redid,IPC_RMID, NULL);
    
    return 0;
}
