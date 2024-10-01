/* Linux program koji kreira dodatna dva procesa deteta. Prvi proces dete predstavlja amfiteatar A1, a drugi proces dete amfiteatar A2.
 * Roditelj sa svakim dete procesom komunicira koriscenjem istog reda poruka tako da svako dete cita poruke namenjene samo njemu. 
 * Proces roditelj iz fajla prijava.txt cita redove koji sadrze ime, prezime i broj indeksa u obliku ime prezime br_indeksa. Ukoliko je 
 * broj indeksa manji od 18000, podatke o studentu proces roditelj salje procesu detetu A1 inace salje procesu detetu A2. Nakon sto je 
 * spisak procitan komnikacija se prekida. Svaki proces dete treba da ispise minimalnu i maksimalnu vrednost broja indeksa koje je dobio 
 * od procesa roditelja.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSGLEN 100
#define QUEUE_KEY 10104

struct mymsgbuf {
    long mtype;
    char mtext[MSGLEN];
};

int main() {
    int msqid;
    struct mymsgbuf buf;
    FILE *file;
    int pid1, pid2;
    
    msqid = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if (msqid == -1) 
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Kreiramo dva procesa deteta
    pid1 = fork();
    if (pid1 == 0) 
    {  // Proces A1
        int min = 18000, max = 0;
        while (1) {
            msgrcv(msqid, &buf, MSGLEN, 1, 0);  // Prima poruke sa mtype = 1
            if (strcmp(buf.mtext, "quit") == 0) 
            {
                break;
            }
            int br_indeksa = atoi(strrchr(buf.mtext, ' ') + 1);  // Ekstrahuje broj indeksa
            if (br_indeksa < min) 
                min = br_indeksa;
            if (br_indeksa > max) 
                max = br_indeksa;
        }
        printf("A1: Minimalni broj indeksa: %d, Maksimalni broj indeksa: %d\n", min, max);
        exit(0);
    } 
    else 
    {
        pid2 = fork();
        if (pid2 == 0) 
        {  // Proces A2
            int min = 99999, max = 18000;
            while (1) 
            {
                msgrcv(msqid, &buf, MSGLEN, 2, 0);  // Prima poruke sa mtype = 2
                if (strcmp(buf.mtext, "quit") == 0) {
                    break;
                }
                int br_indeksa = atoi(strrchr(buf.mtext, ' ') + 1);  // Ekstrahuje broj indeksa
                if (br_indeksa < min) min = br_indeksa;
                if (br_indeksa > max) max = br_indeksa;
            }
            printf("A2: Minimalni broj indeksa: %d, Maksimalni broj indeksa: %d\n", min, max);
            exit(0);
        } 
        else 
        {  // Proces roditelj
            file = fopen("prijava.txt", "r");
            if (file == NULL) 
            {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            char line[MSGLEN];
            while (fgets(line, sizeof(line), file)) 
            {
                line[strcspn(line, "\n")] = '\0';  // Uklanja novi red sa kraja linije
                int br_indeksa = atoi(strrchr(line, ' ') + 1);  // Ekstrahuje broj indeksa
                
                if(br_indeksa < 18000)
                    buf.mtype = 1
                else
                    buf.mtype = 2;  // Postavlja tip poruke
                    
                strcpy(buf.mtext, line);
                msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);
            }
            fclose(file);

            // Šalje "quit" poruke detetima da završe sa radom
            strcpy(buf.mtext, "quit");
            buf.mtype = 1;
            msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);
            buf.mtype = 2;
            msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);

            wait(NULL);  // Čekamo da oba procesa deteta završe
            wait(NULL);

            msgctl(msqid, IPC_RMID, NULL);  // Brišemo red poruka
        }
    }
    return 0;
}
