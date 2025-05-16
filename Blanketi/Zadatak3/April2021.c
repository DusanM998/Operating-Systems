/*Linux program koji kreira dodatni proces sa kojim komunicira dvosmerno koriscenjem jednog reda poruka. Proces roditelj
 * iz datoteke cije je ime prosledjeno kao argument komandne linije cita liniju po liniju teksta i svaku liniju salje procesu detetu
 * u posebnoj poruci. Proces dete u svakoj primljenoj liniji prebrojava broj pojavljivanja velikih slova abecede i procesu roditelju 
 * vraca kroz isti red poruku za svaku analiziranu liniju teksta kroz posebnu poruku redni broj linije i broj pojavljivanja velikih slova.
 * Glavni program te podatke stmpa na ekranu(redni broj linije i broj pojavljivanja velikih slova). Kada se analiziraju sve linije teksta
 * iz datoteke glavni program na ekranu treba da odstampa "KRAJ".*/

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
#define MAX_PORUKA 256

//Struktura za poruku
struct msgbuf 
{
    long mtype;
    char mtext[MAX_PORUKA];
};

//Struktura poruke koja sadrzi rezultat
struct result_msgbuf
{
    long mtype;
    int line_num;
    int brojVelikih;
};

int racunaj_broj_velikih(char *str)
{
    int count = 0;
    for(int i=0; str[i] != '\0'; i++)
    {
        if(isupper(str[i]))
            count++;
    }
    return count;
}

int main(int argc, char *argv[])
{
    int pid;
    int msqid;
    FILE *fileCitanje;
    
    if(argc < 2)
    {
        printf("Nema dovoljno argumenata za pokretanje procesa!\n");
        fprintf(stderr, "Pravilno uneti argumente: %s <source_file>\n", argv[0]);
    }
    
    msqid = msgget(RED_PORUKA, IPC_CREAT | 0666);
    if(msqid < 0)
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
    
    if(pid == 0)
    {
        //Proces dete prebrojava broj velikih slova
        struct msgbuf poruka;
        struct result_msgbuf rezultat;
        
        while(1)
        {
            //Primanje poruke od roditelja
            if(msgrcv(msqid, &poruka, sizeof(poruka.mtext),1, 0) == -1)
            {
                perror("Greska prilikom primanja poruke!\n");
                exit(1);
            }
            
            //Provera da li je kraj
            if(strcmp(poruka.mtext, "KRAJ") == 0)
                break;
            
            int brojVelikih = racunaj_broj_velikih(poruka.mtext);
            
            //Slanje rezultata procesu roditelju
            rezultat.mtype = 2;
            rezultat.line_num = atoi(poruka.mtext);
            rezultat.brojVelikih = brojVelikih;
            
            if(msgsnd(msqid, &rezultat, sizeof(rezultat), 0) == -1)
            {
                perror("Greska pri slanju rezultata roditelju!\n");
                exit(1);
            }
        }
        exit(0);
        
    }
    else
    {
        //Proces roditelj: iz datoteke cije je ime prosledjeno kao argument cita liniju po liniju
        //i prosledjuje je procesu detetu
        struct msgbuf poruka;
        struct result_msgbuf rezultat;
        fileCitanje = fopen(argv[1], "r");
        
        if(fileCitanje == NULL)
        {
            printf("Doslo je do greske prilikom otvaranja datoteke!\n");
            exit(1);
        }
        
        int brojLinije = 0;
        char linija[MAX_PORUKA];
    
        while(fgets(linija, sizeof(linija), fileCitanje))
        {
            poruka.mtype = 1; //Tip poruke za dete
            snprintf(poruka.mtext, sizeof(poruka.mtext), "%d %s", brojLinije, linija);
            
                
            if(msgsnd(msqid, &poruka, strlen(poruka.mtext) + 1, 0) == -1)
            {
                printf("Doslo je do greske prilikom slanja poruke!\n");
                exit(1);
            }
            
            if(msgrcv(msqid, &rezultat, sizeof(rezultat), 2, 0) == -1)
            {
                printf("Doslo je do greske prilikom primanja rezultata od deteta!\n");
                exit(1);
            }
            
            printf("Proces roditelj: Linija %d: Broj velikih slova = %d\n", rezultat.line_num, rezultat.brojVelikih);
            
            brojLinije++;
        }
        fclose(fileCitanje);
        
        strcpy(poruka.mtext, "KRAJ");
        if(msgsnd(msqid, &poruka, strlen(poruka.mtext) + 1, 0) == -1)
        {
            perror("Greska prilikom slanja poruke KRAJ detetu!\n");
            exit(1);
        }
        
        wait(NULL); //Cekanje da se proces dete zavrsi
        
        msgctl(msqid, IPC_RMID, NULL);
        
        printf("KRAJ\n");
    }
    
    return 0;
}
