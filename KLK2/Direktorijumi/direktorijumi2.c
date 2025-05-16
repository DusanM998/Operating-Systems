#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void processDat(char* fileName);
void processDir(char* fileName);

int main(int argc, char* argv[])
{
    struct stat podaci;
    int retVal;
    if(argc < 2)
    {
        printf("Pogresni ulazni argument!\n");
        exit(-1);
    }
    
    printf("Ulazni argument: %s\n",argv[1]);
    
    retVal = stat(argv[1], &podaci);
    if(retVal == 1)
    {
        printf("Pogresni ulazni argument!\n");
        exit(-1);
    }
    
    if(S_ISDIR(podaci.st_mode))
    {
        processDir(argv[1]);
    }
    
    exit(0);
}

void processDir(char* fileName)
{
    DIR* direktorijum;
    struct dirent* stavka;
    struct stat status;
    char stavkaPath[1024];
    int retVal;
    
    printf("Obradjujem direktorijum: %s\n", fileName);
    direktorijum = opendir(fileName);
    if(direktorijum == NULL)
    {
        return;
    }
    
    while((stavka = readdir(direktorijum))!=NULL)
    {
        strcpy(stavkaPath, "");
        strcpy(stavkaPath, fileName);
        strcpy(stavkaPath, stavka->d_name);
        sleep(2);
        stat(stavkaPath, &status);
        
        if(strcmp(stavka->d_name,"..")!=0 && strcmp(stavka->d_name,".")!=0)
        {
            if(S_ISDIR(status.st_mode))
            {
                strcat(stavkaPath, "/");
                processDir(stavkaPath);
            }
            else if(S_ISREG(status.st_mode))
            {
                processDat(stavkaPath);
            }
        }
    }
    closedir(direktorijum);
}

void processDat(char* fileName)
{
    printf("Obradjena datoteka: %s\n", fileName);
}
