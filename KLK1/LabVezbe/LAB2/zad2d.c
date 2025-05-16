#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 100

int main()
{
    char buf[BUF_SIZE];
    int i;
    FILE* f;
    int print_flag = 0;
    
    for(i = 0; i < BUF_SIZE; i++)
        buf[i] = '\0';
    
    f = fopen("/proc/devices", "r");
    
    if(!f)
    {
        printf("Doslo je do greske prilokom otvaranja datoteke sa informacijama\n");
        return -1;
    }
    
    while(!feof(f))
    {
        if(fgets(buf, BUF_SIZE, f))
        {
            if(strstr(buf, "Block devices:"))
                print_flag = 1;
            if(print_flag == 1)
                printf("%s", buf);
        }
    }
    
    fclose(f);
    
    return 0;
}
