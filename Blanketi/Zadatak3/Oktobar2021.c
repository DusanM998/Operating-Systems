/*Linux program koji kreira dodatni proces dete. Proces roditelj sa standardnog ulaza prihvata podatke o studentima
 * (broj indeksa, ime i prezime studenta) i koriscenjem reda poruka te informacije salje procesu detetu. Proces dete
 * ima zadatak, da nakon sto primi podatke o poslednjem studentu, sortira studente po broju indeksa i tako dobijeni
 * spisak odstampa na standardnom izlazu.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MAX_STUDENTI 100
#define MSGLEN 255
#define RED_PORUKA 12345

struct student 
{
    int broj_indeksa;
    char ime_prezime[100];
};

struct mymsgbuf 
{
    long mtype;
    struct student stud;
};

void sort_studenti(struct student studenti[], int n) 
{
    struct student temp;
    for (int i = 0; i < n - 1; i++) 
    {
        for (int j = i + 1; j < n; j++) 
        {
            if (studenti[i].broj_indeksa > studenti[j].broj_indeksa) 
            {
                temp = studenti[i];
                studenti[i] = studenti[j];
                studenti[j] = temp;
            }
        }
    }
}

int main() 
{
    int msqid;
    struct mymsgbuf buf;
    struct student studenti[MAX_STUDENTI];
    int broj_studenata = 0;
    
    // Kreiranje reda poruka
    msqid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    if(msqid == -1) 
    {
        printf("Greska prilikom kreiranja reda poruka!\n");
        exit(1);
    }
    
    if(fork() == 0) 
    { 
        // Proces dete: nakon sto primi podatke o poslednjem studentu
        //Sortira studente prema broju indeksa
        while(1) 
        {
            // Prijem podataka o studentu
            msgrcv(msqid, &buf, sizeof(struct student), 1, 0);
            
            if(buf.stud.broj_indeksa == -1) 
            {  // Kraj unosa
                break;
            }
            
            // Čuvanje primljenih podataka o studentu
            studenti[broj_studenata] = buf.stud;
            broj_studenata++;
        }

        // Sortiranje studenata prema broju indeksa
        sort_studenti(studenti, broj_studenata);

        // Ispis sortirane liste studenata
        printf("Sortirani studenti po broju indeksa:\n");
        for (int i = 0; i < broj_studenata; i++) 
        {
            printf("Indeks: %d, Ime i prezime: %s\n", studenti[i].broj_indeksa, studenti[i].ime_prezime);
        }

        exit(0);
    } 
    else 
    { 
        // Proces roditelj - Unosi podatke o studentima (br. indeksa, ime i prezime)
        while(1) 
        {
            // Unos podataka o studentu
            printf("Unesite broj indeksa (ili -1 za kraj): ");
            scanf("%d", &buf.stud.broj_indeksa);

            if (buf.stud.broj_indeksa == -1) 
            {  // Ako je kraj
                buf.mtype = 1;
                msgsnd(msqid, &buf, sizeof(struct student), 0); // Slanje poruke za kraj
                break;
            }

            printf("Unesite ime i prezime: ");
            getchar();
            fgets(buf.stud.ime_prezime, sizeof(buf.stud.ime_prezime), stdin);
            buf.stud.ime_prezime[strcspn(buf.stud.ime_prezime, "\n")] = '\0';  // Uklanjanje newline karaktera

            // Slanje poruke procesu detetu
            buf.mtype = 1;
            msgsnd(msqid, &buf, sizeof(struct student), 0);
        }

        wait(NULL);  // Cekanje da proces dete završi

        // Brisanje reda poruka
        msgctl(msqid, IPC_RMID, NULL);
    }

    return 0;
}
