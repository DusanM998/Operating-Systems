/*Linux program koji kreira dodatni proces dete. Proces roditelj startuje proces dete i salje mu svoje argumente komandne linije koriscenjem execl sistemskog poziva tako da su argumenti prosledjeni u obrnutom redosledu.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int pid;
    char* arguments[argc];
    
    int i=0;
    for(int j=argc-1; j>=0; j--)
    {
        arguments[i] = (char*)malloc((strlen(argv[j])+1)*sizeof(char));
        strcpy(arguments[i], argv[j]);
        printf("%s\n", arguments[i]);
        i++;
    }
    char parametar[100];
    sprintf(parametar, "%d", i);
    pid = fork();
    if(pid == 0)
    {
        execl("./dete", "./dete", parametar, arguments, NULL);
        printf("Neuspesno pokretanje procesa deteta!\n");
        exit(-1);
    }
    else
    {
        wait(NULL);
    }
}
