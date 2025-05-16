#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 100

int main(int argc, char* argv[])
{
    char buf[MAX_BUF], buf1[MAX_BUF], buf2[MAX_BUF];
    FILE* f;
    int i;
    
    for(i=0; i < MAX_BUF; i++)
    {
        buf[i] = '\0';
        buf1[i] = '\0';
        buf2[i] = '\0';
    }
    
    f = fopen("/proc/self/status", "r");
    
    if(f == 0) //do greske dolazi ukoliko nemamo privilegije za pojedine fajlove
    {
        printf("Doslo je do greske priloikom pristupanja informacijama!\n");
        return -1;
    }
    
    while(!feof(f))
    {
        fscanf(f, "%s", buf);
        
        if(strcmp(buf, "Pid:") ==0)
        {
            fscanf(f, "%s", buf1);
            printf("Pid je: %s\n", buf1);
        }
        
        if(strcmp(buf, "PPid:") == 0)
        {
            fscanf(f, "%s\n", buf2);
            printf("PPID je: %s\n", buf2);
        }
    }
    
    fclose(f);
    
    return 0;
}
