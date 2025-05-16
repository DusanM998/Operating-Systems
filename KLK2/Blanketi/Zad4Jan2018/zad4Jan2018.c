/*Januar 2018*/
/*Linux program koji pocevci od korenskog(root) direktorijuma, pronalazi i stampa sve regularne datoteke cija je apsolutna putanja duza od 256 karaktera i cija je velicina manja od 2KB*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

void processDir(char* dirPath);

int main(int argc, char* argv[])
{
    char pocetnaPutanja[10];
    strcpy(pocetnaPutanja, "/");
    struct stat status;
    int retVal;
    
    retVal = stat(pocetnaPutanja, &status);
    if(retVal == -1)
    {
        printf("Neuspesan 1. stat!\n");
        exit(-1);
    }
    
    processDir(pocetnaPutanja);
}

void processDir(char* dirPath)
{
    DIR* direktorijum;
    struct dirent* stavka;
    struct stat status;
    char stavkaPath[100000];
    
    direktorijum = opendir(dirPath);
    while((stavka = readdir(direktorijum)) != NULL)
    {
        if(strcmp(stavka->d_name, ".") !=0 && strcmp(stavka->d_name, "..") != 0)
        {
            strcpy(stavkaPath, dirPath);
            strcat(stavkaPath, stavka->d_name);
            printf("Obradjujem: %s\t%ld\n", stavkaPath, strlen(stavkaPath));
            
            stat(stavkaPath, &status);
            if(S_ISDIR(status.st_mode))
            {
                strcat(stavkaPath, "/");
                processDir(stavkaPath);
            }
            else if(S_ISREG(status.st_mode))
            {
                if(status.st_size < 2000 && strlen(stavkaPath) > 256)
                {
                    printf("%s\n", stavkaPath);
                }
            }
        }
    }
}
