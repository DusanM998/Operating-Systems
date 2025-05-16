/*Oktobar2018 (zadatak2)*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    printf("%s\n", argv[0]);
    int count = atoi(argv[1]);
    printf("%d\n", count);
    
    for(int i=0; i < count; i++)
        printf("%s\n", (char*)argv[2]);
}
