/*Linux program koji u zadatom direktorijumu (putanja do direktorijuma se zadaje kao ulazni argument programa) rekurzivno nalazi 5 najvecih regularnih datoteka. Program na kraju reda treba da odstampa putanje i velicine tih datoteka*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

struct regdat
{
    int size;
    char filePath[10000];
};

struct regdat niz[5];
int counter = 0;


void processDirectory(char* directoryPath);

void sortirajNiz()
{
    for(int i=0; i < 4; i++)
        for(int j=i+1; j < 5; j++)
        {
            if(niz[j].size < niz[i].size)
            {
                struct regdat pom;
                
                pom.size = niz[j].size;
                strcpy(pom.filePath, niz[j].filePath);
                
                niz[j].size = niz[i].size;
                strcpy(niz[j].filePath, niz[i].filePath);
                
                niz[i].size = pom.size;
                strcpy(niz[i].filePath, pom.filePath);
            }
        }
}

int main(int argc, char* argv[])
{
    struct stat status;
    int retVal;
    if(argc < 2)
    {
        printf("Pogresan broj ulaznih argumenata!\n");
        exit(-1);
    }
    retVal = stat(argv[1], &status);
    if(retVal == -1)
    {
        printf("Greska pri prvom statu!\n");
        exit(-1);
    }
    if(S_ISDIR(status.st_mode))
    {
        for(int i=0; i < 5; i++)
            niz[i].size = -1;
        processDirectory(argv[1]);
        
        for(int i=0; i < 5; i++)
            printf("%s %d\n", niz[i].filePath,niz[i].size);
    }
    else
    {
        printf("Ulazni argument ne predstavlja datoteku!\n");
    }
}

void processDirectory(char* directoryPath)
{
    DIR* direktorijum;
    struct stat status;
    struct dirent* stavka;
    char stavkaPath[10000];
    
    
    direktorijum = opendir(directoryPath);
    while((stavka = readdir(direktorijum)) != NULL)
    {
        if(strcpy(stavka->d_name, ".") != 0 && strcmp(stavka->d_name, "..")!=0)
        {
            strcpy(stavkaPath, directoryPath);
            strcat(stavkaPath, stavka->d_name);
            
            stat(stavkaPath, &status);
            if(S_ISDIR(status.st_mode))
            {
                strcat(stavkaPath, "/");
                processDirectory(stavkaPath);
            }
            else if(S_ISREG(status.st_mode))
            {
                printf("Proveravam: %s\n", stavkaPath);
                if(counter < 5)
                {
                    niz[counter].size = status.st_size;
                    strcpy(niz[counter].filePath, stavkaPath);
                    counter++;
                }
                else if(counter == 5)
                {
                    sortirajNiz();
                    counter++;
                    
                    int brojac=4;
                    while(brojac >= 0)
                    {
                        if(status.st_size>=niz[brojac].size)
                        {
                            if(brojac == 0)
                            {
                                niz[brojac].size = status.st_size;
                                strcpy(niz[brojac].filePath, stavkaPath);
                                brojac--;
                            }
                            else
                            {
                                for(int i=0; i < brojac; i++)
                                {
                                    niz[i].size = niz[i+1].size;
                                    strcpy(niz[i].filePath, niz[i+1].filePath);
                                }
                                niz[brojac].size = status.st_size;
                                strcpy(niz[brojac].filePath, stavkaPath);
                                brojac--;
                                break;
                            }
                        }
                        else
                        {
                            int brojac = 4;
                            while(brojac >= 0)
                            {
                                if(status.st_size >= niz[brojac].size)
                                {
                                    if(brojac == 0)
                                    {
                                        niz[brojac].size = status.st_size;
                                        strcpy(niz[brojac].filePath, stavkaPath);
                                        brojac--;
                                    }
                                    else
                                    {
                                        for(int i=0; i < brojac; i++)
                                        {
                                            niz[i].size = niz[i+1].size;
                                            strcpy(niz[i].filePath, niz[i+1].filePath);
                                        }
                                        niz[brojac].size = status.st_size;
                                        strcpy(niz[brojac].filePath, stavkaPath);
                                        brojac--;
                                        break;
                                    }
                                }
                                else
                                    brojac--;
                            }
                        }
                    }
                }
            }
        }
    }
}
                                    
                        
                            
