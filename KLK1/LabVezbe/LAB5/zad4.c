/*Linux program koji kao ulazni argument prihvata putanju do jedne izvrsne datoteke. Program treba da pozove na izvrsenje tako prosledjenu izvrsnu datoteku i njen izlaz prikazuje na std izlazu ekran po ekran. Ovo se postize tako se se izlaz pozvane izvrsne datoteke prosledjuje programu more, koriscenjem datavoda*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>

int main(int argc, char* argv[])
{
    int pd[2];
    int pid;
    char* args[10];
    int i;
    
    if(pipe(pd) < 0)
    {
        exit(1);
    }
    pid = fork();
    if(pid == 0)
    {
        close(1); //Ukoliko je proces dete prebacujemo njegov izlaz na datavod
        dup(pd[1]); 
        
        for(i=1; i < argc; i++)
            args[i-1] = argv[i];
        args[argc - 1] = NULL;
        
        execvp(args[0], args);
    }
    else //proces roditelj zatvara std ulaz i povezuje ga na stranu pipe-a koji se upisuje
    {
        close(0);
        dup(pd[0]);
        close(pd[1]);
        execlp("more", "more", NULL);
    }
}
