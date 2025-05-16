/*Linux program koji za kopiranje datoteke kreira dodatni proces dete. Ime datoteke koja se kopira i ime odredisne datoteke 
 * prosledjuju se kao argumenti komandne linije. Komunikacija izmedju glavnog procesa i novokreiranog procesa deteta se odvija
 * koriscenjem mehanizma datavoda. Glavni proces najpre procesu detetu prosledjuje ime odredisne datoteke. Nakon toga, glavni proces
 * podatke iz odredisne datoteke, u paketima od po 200 bajtova, prosledjuje procesu detetu. Proces dete primljene bajtove upisuje u odredisnu datoteku.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 200

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pd[2];
    int pid;

    if (pipe(pd) < 0) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) 
    { 
        // Child process: prima ime datoteke i upisuje podatke u odredišnu datoteku
        close(pd[1]); // Zatvaramo kraj za pisanje

        // Primamo ime odredišne datoteke
        char dest_file[256];
        read(pd[0], dest_file, sizeof(dest_file));

        // Otvaramo datoteku za pisanje
        int fd_dest = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_dest < 0) 
        {
            perror("open destination file");
            close(pd[0]);
            exit(EXIT_FAILURE);
        }

        // Primamo i upisujemo podatke u odredišnu datoteku
        char buffer[BUFFER_SIZE];
        int bytes_read;
        while ((bytes_read = read(pd[0], buffer, BUFFER_SIZE)) > 0) 
        {
            if (write(fd_dest, buffer, bytes_read) != bytes_read) 
            {
                perror("write to destination file");
                close(pd[0]);
                close(fd_dest);
                exit(EXIT_FAILURE);
            }
        }

        close(pd[0]);
        close(fd_dest);
        exit(0);
    } 
    else 
    { 
        // Parent process: šalje ime datoteke i podatke procesu detetu
        close(pd[0]); // Zatvaramo kraj za čitanje

        // Šaljemo ime odredišne datoteke detetu
        write(pd[1], argv[2], strlen(argv[2]) + 1);

        // Otvaramo izvornu datoteku za čitanje
        int fd_src = open(argv[1], O_RDONLY);
        if (fd_src < 0) 
        {
            perror("open source file");
            close(pd[1]);
            exit(EXIT_FAILURE);
        }

        // Čitamo i šaljemo podatke detetu u paketima od po 200 bajtova
        char buffer[BUFFER_SIZE];
        int bytes_read;
        while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0) 
        {
            if (write(pd[1], buffer, bytes_read) != bytes_read) 
            {
                perror("write to pipe");
                close(pd[1]);
                close(fd_src);
                exit(EXIT_FAILURE);
            }
        }

        close(fd_src);
        close(pd[1]); // Zatvaramo kraj za pisanje nakon što su svi podaci poslani

        wait(NULL); // Čekamo da proces dete završi
    }

    return 0;
}
