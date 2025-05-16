/*Linux program koji u zadatom direktorijumu lista nazive svih datoteka koje u svom nazivu sadrze zadati string. Putanja do direktorijuma i string koji se trazi, zadaju se kao argument komandne linije.*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char* argv[])
{
    //DIR struktura(ona koju vrati opendir f-ja)
    DIR* dp;
    struct dirent* dirp;
    
    if(argc != 0)
    {
        printf("Neophodna su 2 argumenta (putanja do direktorijuma i string koji se trazi!\n");
        exit(-1);
    }
    
    if((dp = opendir(argv[1])) == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma %s!\n", argv[1]);
        exit(-1);
    }
    
    //readdir vraca pokazivac na strukturu tipa dirent
    while( (dirp = readdir(dp)) != NULL)
    {
        if(strstr(dirp->d_name, argv[2]) != NULL)
            printf("%s\n", dirp->d_name);
    }
    closedir(dp);
}
