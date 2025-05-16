/*Linux program koji za kopiranje datoteke kreira dodatni proces dete. Ime datoteke koja se kopira i ime odredisne datoteke se prosledjuju kao argumenti komandne linije. Komunikacija izmedju glavnog procesa i novokreiranog procesa deteta se odvija koriscenjem mehanizma datavoda. Glavni proces najpre procesu detetu prosledjuje ime odredisne datoteke. Nakon toga, glavni proces podatke iz odredisne datoteke, u paketima od po 200 bajtova, prosledjuje procesu detetu. Proces dete primljene bajtove upisuje u odredisnu datoteku*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[])
{
    //Prvi argument komandne linije je izvorisna datoteka, a drugi odredisna
    
    int pid;
    int datavod[2];
    FILE* f1;
    FILE* f2;
    char red[200];
    char odredisna[1000];
    int velicinaPutanje = 0;
    
    if(pipe(datavod) < 0)
    {
        printf("Neuspesno otvaranje datavoda!\n");
        exit(-1);
    }
    
    pid = fork();
    if(pid == 0)
    {
        close(datavod[1]);
        read(datavod[0], &velicinaPutanje, sizeof(int));
        read(datavod[0], odredisna, velicinaPutanje);
        
        f2 = fopen(odredisna, "w");
        
        while(read(datavod[0], red, 200) > 0)
            fputs(red, f2);
        
        fclose(f2);
        close(datavod[0]);
        exit(0);
    }
    else
    {
        close(datavod[0]);
        velicinaPutanje = strlen(argv[2]) + 1;
        write(datavod[1], &velicinaPutanje, sizeof(int));
        write(datavod[1], argv[2], (strlen(argv[2]) + 1)*sizeof(char));
        
        f1 = fopen(argv[1], "r");
        fgets(red, 200, f1);
        while(!feof(f1))
        {
            write(datavod[1], red, 200);
            fgets(red, 200, f1);
        }
        
        close(datavod[1]);
        
        wait(NULL);
        printf("Finished!\n");
        fclose(f1);
        exit(0);
    }
}
