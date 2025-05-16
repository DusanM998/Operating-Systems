#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <dirent.h>

int main(int arg, char* argv[])
{
    DIR* direktorijum;
    struct dirent* stavkaDirektorijuma;
    
    direktorijum = opendir("/home/dusan98/Desktop/Operativni/KLK2/Direktorijumi/");
    if(direktorijum == NULL)
    {
        printf("Neuspesno otvaranje datoteke!");
        exit(0);
    }
    
    while((stavkaDirektorijuma = readdir(direktorijum))!= NULL)
    {
        printf("%s\n", stavkaDirektorijuma->d_name);
    }
    
    closedir(direktorijum);
}
