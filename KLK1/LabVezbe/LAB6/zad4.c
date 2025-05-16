/*Linux program koji svom procesu detetu, koriscenjem redova poruka, prosledjuje ulaz koji prima preko tastature, a proces dete dobijene poruke stampa na ekranu. Predvideti da se unosom teksta "QUIT" prekida rad programa*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>

#define MSGLEN 50

//Struktura koja drzi poruke
struct mymsgbuf
{
    long mtype;
    char mtext[MSGLEN];
};

int main()
{
    int msqid;
    struct mymsgbuf buf;
    
    buf.mtype = 1;
    strcpy(buf.mtext, "");
    
    msqid = msgget(10104, 0666 | IPC_CREAT); //Red poruka koji dobijamo pomocu f-je msgget
    
    if(fork() == 0)
    {
        while(strcmp(buf.mtext, "quit") != 0) //Proverava da li je uzeta poruka quit
        {
            msgrcv(msqid, &buf, 50, 0/*sve poruke*/, 0/*blokirajuci poziv*/);
            printf("Proces dete: %s\n", buf.mtext);
        }
    }
    else
    {
        while(strcmp(buf.mtext, "quit") != 0)
        {
            scanf("%s", buf.mtext);
            msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0/*blokirajuci*/);
        }
        
        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL); /*F-ja koja izvrsava neku kontrolnu operaciju specificiranu 2. argumentom (IPC_RMID uklanja red poruka i sve njemu pridruzene strukture podataka)*/
    }
    return 0;
}
