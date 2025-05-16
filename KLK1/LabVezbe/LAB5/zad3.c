/*Linux program koji demonstrira mogucnosti redirekcije std izlaza i ulaza. Program prihvata 2 ulazna argumenta: putanju do izvrsne datoteke i putanju do txt datoteke. Program otvara txt datoteku i redirektuje standardni izlaz na tu otvorenu datoteku. Efekat ovog programa je da se izlaz svakog pozvanog programa redirektuje u izabranu txt datoteku.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/file.h>

int main(int argc, char* argv[])
{
    int fd;
    
    /*Otvara txt datoteku*/
    fd = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0666);
    
    /*Redirektuje std izlaz na otvorenu txt datoteku*/
    dup2(fd, 1); //isto sto i: close(1); dup(fd); (dup najazi najmanju vr. fajl desktiptora koja postoji*/
    
    /*Zatvara desktiptor za datoteku (kopija ostaje)*/
    close(fd);
    
    /*Poziva izvrsnu datoteku*/
    execlp(argv[1], argv[1], NULL);
    printf("Ne bi trebalo nikad da se izvsi\n");
}
