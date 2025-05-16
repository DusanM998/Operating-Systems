#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF 100

int main(int argc, char* argv[])
{
    char buf[MAX_BUF];
    FILE* f;
    int i;
    
    for(i=0; i < MAX_BUF; i++)
        buf[i] = '\0';
    
    f = fopen("/proc/1/status", "r");
    
    if(f == 0)
    {
        printf("Doslo je do greske prilikom pristupanja informacijama -- proc!\n");
        return -1;
    }
    
    while(!feof(f))
    {
        if(fgets(buf, MAX_BUF, f))
            printf("%s", buf);
    }
    
    fclose(f);
    
    f = fopen("/proc/1/maps", "r");
    
    if(f == 0)
    {
        printf("Doslo je do greske prilikom pristupanja informacijama --- maps!\n");
        return -1;
    }
    
    while(!feof(f))
    {
        if(fgets(buf,MAX_BUF,f))
            printf("%s",buf);
    }
    
    fclose(f);
    
    return 0;
}
