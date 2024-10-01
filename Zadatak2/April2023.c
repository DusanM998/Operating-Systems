/*Linux program koji kreira dodatni proces dete. U dodatnom procesu startuje program
 * ciji je naziv zadat kao prvi argument komandne linije. Startovanom programu prosledjuje
 * svoj drugi argument komandne linije. Glavni proces ceka da se proces dete zavrsi i na ekranu stampa
 * kod sa kojim je proces dete izasao.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <program_name> <argument>\n", argv[0]);
        exit(1);
    }

    int pid = fork();

    if (pid < 0) {  // Greška pri kreiranju deteta
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {  // Proces dete
        // Pokretanje programa sa argumentom
        execlp(argv[1], argv[1], argv[2], (char *)NULL);

        // Ako execlp ne uspe, ispisuje grešku i završava dete
        perror("execlp failed");
        exit(1);
    } else {  // Glavni proces
        int status;
        
        // Čekanje da se proces dete završi
        waitpid(pid, &status, 0);

        // Provera da li je dete završeno normalno
        if (WIFEXITED(status)) {
            printf("Child process exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process did not exit normally\n");
        }
    }

    return 0;
}
