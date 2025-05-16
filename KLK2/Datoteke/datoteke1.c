#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    int fd;
    char buffer[1024];
    int readedBytes;
    
    if(argc < 2)
    {
        printf("Pogresni ulazni parametri!\n");
        exit(-1);
    }
    
    fd = open(argv[1],O_RDONLY);
    
    if(fd < 0)
    {
        printf("Neuspesno otvaranje datoteke!\n");
        exit(-1);
    }
    
    while((readedBytes = read(fd, buffer, 1023)) > 0)
    {
        printf("%s", buffer);
    }
    
    close(fd);
    exit(0);
}
