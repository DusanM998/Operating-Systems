/*Linux program koji kreira dva dodatna procesa. Drugi proces cita tekstualnu datoteku prva.txt red po red i tako procitane
 * redove salje procesu roditelju preko reda poruka. Treci proces cita tekstualnu datoteku druga.txt red po red i tako procitane
 * redove teksta salje roditeljskom procesu koristeci isti red poruka. Roditeljski proces sva slova u recenicama iz datoteke prva.txt
 * pretvara u velika slova, a sva slova iz datoteke druga.txt u mala slova i tako modifikovane upisuje u datoteke prva_mod.txt i druga_mod.txt.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <sys/wait.h>

#define RED_PORUKA 10401
#define MAX_LINE_LEN 1024
#define END_OF_FILE_MSG 3  // Tip poruke koji označava kraj slanja

// Struktura za poruku
struct mymsgbuf {
    long mtype;                 // Tip poruke (1 za prva.txt, 2 za druga.txt, 3 za kraj)
    char mtext[MAX_LINE_LEN];   // Tekst poruke (red iz datoteke)
};

// Funkcija za pretvaranje teksta u velika slova
void to_uppercase(char *str) 
{
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Funkcija za pretvaranje teksta u mala slova
void to_lowercase(char *str) 
{
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int main() {
    int msgid;
    struct mymsgbuf buf;
    
    // Kreiranje reda poruka
    msgid = msgget(RED_PORUKA, 0666 | IPC_CREAT);
    if (msgid == -1) 
    {
        perror("Greska prilikom kreiranja reda poruka");
        exit(1);
    }

    int pid1 = fork();
    
    if (pid1 == 0) 
    {
        // Drugi proces - citanje iz prva.txt
        FILE *file = fopen("prva.txt", "r");
        if (file == NULL) 
        {
            perror("Greska prilikom otvaranja datoteke prva.txt");
            exit(1);
        }

        while (fgets(buf.mtext, MAX_LINE_LEN, file)) 
        {
            buf.mtype = 1;  // Tip poruke za prva.txt
            msgsnd(msgid, &buf, sizeof(buf.mtext), 0);
        }

        // Slanje signalne poruke za kraj
        buf.mtype = END_OF_FILE_MSG;
        msgsnd(msgid, &buf, sizeof(buf.mtext), 0);

        fclose(file);
        exit(0);
    }

    int pid2 = fork();
    
    if (pid2 == 0) 
    {
        // Treci proces - citanje iz druga.txt
        FILE *file = fopen("druga.txt", "r");
        if (file == NULL) 
        {
            perror("Greska prilikom otvaranja datoteke druga.txt");
            exit(1);
        }

        while (fgets(buf.mtext, MAX_LINE_LEN, file)) 
        {
            buf.mtype = 2;  // Tip poruke za druga.txt
            msgsnd(msgid, &buf, sizeof(buf.mtext), 0);
        }

        // Slanje signalne poruke za kraj
        buf.mtype = END_OF_FILE_MSG;
        msgsnd(msgid, &buf, sizeof(buf.mtext), 0);

        fclose(file);
        exit(0);
    }

    // Roditeljski proces - obrada i zapisivanje u prva_mod.txt i druga_mod.txt
    FILE *file1 = fopen("prva_mod.txt", "w");
    FILE *file2 = fopen("druga_mod.txt", "w");

    if (file1 == NULL || file2 == NULL) 
    {
        perror("Greska prilikom otvaranja datoteka za upis");
        exit(1);
    }

    int eof_count = 0; // Broj završnih poruka koje smo primili

    while (eof_count < 2) 
    {
        msgrcv(msgid, &buf, sizeof(buf.mtext), 0, 0);

        if (buf.mtype == 1) 
        {
            // Red iz prva.txt - pretvaramo u velika slova
            to_uppercase(buf.mtext);
            fprintf(file1, "%s", buf.mtext);
        } 
        else if (buf.mtype == 2) 
        {
            // Red iz druga.txt - pretvaramo u mala slova
            to_lowercase(buf.mtext);
            fprintf(file2, "%s", buf.mtext);
        } 
        else if (buf.mtype == END_OF_FILE_MSG) {
            eof_count++;
        }
    }

    fclose(file1);
    fclose(file2);

    // Čekanje da procesi završe
    wait(NULL);
    wait(NULL);

    // Brisanje reda poruka
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
