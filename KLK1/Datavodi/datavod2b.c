#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int main(int argc, char* argv[])
{
    char predaja[200];
    int deskriptor = atoi(argv[1]);
    
    printf("Ja sam drugi proces i saljem: \n");
    fgets(predaja, 200, stdin);
    write(deskriptor, predaja, 200*sizeof(char));
    close(deskriptor);
}
