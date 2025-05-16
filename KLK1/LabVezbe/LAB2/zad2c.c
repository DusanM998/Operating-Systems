#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 100

int main(int argc, char* argv[])
{
    char buf[BUF_SIZE];
    char name[BUF_SIZE];
    char size[BUF_SIZE];
    FILE* f;
    int i, j;
    
    f = fopen("/proc/partitions", "r");
    
    if(!f)
    {
        printf("Doslo je do greske prilikom pristupanja datodeci sa informacijama o memoriji!\n");
        return -1;
    }
    
    j=0;

    while(!feof(f))
    {
        if(j > 1)
        {
            for(i=0; i < 4; i++)
            {
                if(i == 2)
                    fscanf(f, "%s", size);
                else if(i == 3)
                    fscanf(f, "%s", name);
                else
                    fscanf(f, "%s", buf);
            }
            
            if(!feof(f))
                printf("%s\t%s\n", name, size);
        }
        else
            fgets(buf, BUF_SIZE, f);
        j++;
    }
    fclose(f);
    
    return 0;
}
