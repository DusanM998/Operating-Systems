#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
    FILE* f;
    char c;
    int brojac = 0;
    
    f = fopen("proba.txt", "r");
    
    while(!feof(f))
    {
        c = fgetc(f);
        brojac++;
    }
    sleep(5);
    printf("Procitani svi karakteri!\n Ima ih: %d\n", brojac);
    exit(0);
}
