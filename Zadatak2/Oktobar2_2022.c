/*Linux program koji kreira dodatni proces dete. Proces roditelj startuje proces dete i
 * salje mu svoje argumente komandne linije koriscenjem execl sistemskog poziva, tako da su 
 * argumenti prosledjeni u obrnutom redosledu*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s arg1 arg2 ...\n", argv[0]);
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) { // Proces dete
        // Obrnuti redosled argumenata
        char *reversed_args[argc + 1]; // +1 za NULL terminator
        reversed_args[0] = argv[0]; // Ime programa ostaje isto

        for (int i = 1; i < argc; i++) {
            reversed_args[i] = argv[argc - i]; // Obrnuto postavljanje argumenata
        }
        reversed_args[argc] = NULL; // NULL terminator

        // Pozivanje execl sa obrnutim argumentima
        execl(argv[0], reversed_args[0], reversed_args[1], reversed_args[2], reversed_args[3], NULL);

        // Ako execl ne uspe
        perror("execl failed");
        exit(1);
    } else { // Proces roditelj
        wait(NULL); // Čekanje da se proces dete završi
    }

    return 0;
}
