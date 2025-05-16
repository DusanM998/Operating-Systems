/*Linux program koji kopira uz kriptovanje datoteku cije se ime prosledjuje kao prvi argument komandne linije
 * tako sto kreira dodatni proces. Glavni proces iz izvorisne datoteke cita 1000 bajta i koriscenjem datavoda
 * procitane bajtove prosledjuje drugom procesu koje te bajtove upisuje u odredisnu datoteku cije se ime prosledjuje
 * kao drugi argument komandne linije, ali tako da svaki bajt XOR-uje rednim brojem bajta u datoteci. 
 * Drugi proces tu slucajno generisanu vrednost prvo upisuje u izlaznu datoteku pa onda bajtove datoteke koja se kopira.
 * Ova procedura se ponavlja dok se ne iskopira kompletna datoteka.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define CHUNK_SIZE 1000

int main(int argc, char *argv[]) {
    // Proveravamo da li su prosleđeni svi potrebni argumenti
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parsiranje argumenata
    const char *source_file = argv[1];
    const char *destination_file = argv[2];

    int pipefd[2];

    // Kreiramo datavod (pipe)
    if (pipe(pipefd) == -1) 
    {
        perror("Greska prilikom kreiranja datavoda");
        exit(EXIT_FAILURE);
    }

    // Kreiramo novi proces pomoću fork()
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("Greska prilikom kreiranja procesa");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) 
    {
        // Child proces: Pisanje u odredišnu datoteku
        close(pipefd[1]); // Zatvaramo kraj za pisanje u datavodu

        int dest_fd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd == -1) 
        {
            perror("Nije moguce otvoriti odredisni fajl!\n");
            exit(EXIT_FAILURE);
        }

        char buffer[CHUNK_SIZE];
        ssize_t bytes_read;
        int byte_index = 0;  // Praćenje apsolutnog indeksa bajta

        printf("Proces dete: Upisujem u odredisni fajl: '%s' sa enkripcijom...\n", destination_file);

        // Čitanje iz datavoda i upisivanje u odredišnu datoteku
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) 
        {
            srand(time(NULL));  // Inicijalizacija generatora nasumičnih brojeva
            char random_value = (rand() % 256);  // Nasumična vrednost bajta

            // Upis nasumične vrednosti u datoteku
            if (write(dest_fd, &random_value, 1) != 1) 
            {
                perror("Greska prilikom upisa u odredisni fajl!\n");
                exit(EXIT_FAILURE);
            }

            // XOR operacija sa apsolutnim rednim brojem bajta i upis u datoteku
            for (int i = 0; i < bytes_read; i++, byte_index++) {
                buffer[i] ^= (unsigned char)byte_index;
            }

            if (write(dest_fd, buffer, bytes_read) != bytes_read) 
            {
                perror("Greska prilikom upisa u odredisni fajl!\n");
                exit(EXIT_FAILURE);
            }
            printf("Proces dete: Upisao %zd bajtova u odredisni fajl.\n", bytes_read);
        }

        if (bytes_read == -1) {
            perror("Error reading from pipe");
        }

        close(pipefd[0]); // Zatvaramo kraj za čitanje iz datavoda
        close(dest_fd); // Zatvaramo datoteku

        printf("Proces dete: Zavrsen upis u odredisni fajl.\n");
        exit(EXIT_SUCCESS);
    } 
    else 
    {
        // Parent proces: Čitanje iz izvorne datoteke
        close(pipefd[0]); // Zatvaramo kraj za čitanje u datavodu

        int src_fd = open(source_file, O_RDONLY);
        if (src_fd == -1) 
        {
            perror("Error opening source file");
            exit(EXIT_FAILURE);
        }

        char buffer[CHUNK_SIZE];
        ssize_t bytes_read;

        printf("Proces roditelj: Citam iz izvorisnog fajla '%s'...\n", source_file);

        // Čitanje iz izvorne datoteke i upisivanje u datavod
        while ((bytes_read = read(src_fd, buffer, CHUNK_SIZE)) > 0) 
        {
            printf("Proces roditelj: Procitao %zd bajtova iz izvorisnog fajla.\n", bytes_read);
            if (write(pipefd[1], buffer, bytes_read) != bytes_read) 
            {
                perror("Error writing to pipe");
                exit(EXIT_FAILURE);
            }
        }

        if (bytes_read == -1) {
            perror("Error reading from source file");
        }

        close(pipefd[1]); // Zatvaramo kraj za pisanje u datavodu
        close(src_fd); // Zatvaramo izvorni fajl

        printf("Proces roditelj: Zavrseno citanje iz izvorisnog fajla.\n");

        // Čekamo da child proces završi
        int status;
        wait(&status);
        if (WIFEXITED(status)) 
        {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        } 
        else 
        {
            printf("Child process did not terminate normally.\n");
        }
    }

    printf("Kopiranje fajla sa enkripcijom zavrseno uspesno.\n");
    return 0;
}
