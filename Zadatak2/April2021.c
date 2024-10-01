/*Linux program koji kopira datoteku cije se ime prosledjuje kao prvi argument komandne linije, tako sto kreira dodatni proces.
 * Glavni proces iz izvorisne datoteke cita N bajtova(N se prosledjuje programu kao drugi argument komandne linije) i koriscenjem datavoda,
 * procitane bajtove prosledjuje drugom procesu koji te bajtove upisuje u odredisnu datoteku cije se ime prosledjuje kao treci argument komandne linije.
 * Ova procedura se ponavlja dok se ne iskopira kompletna datoteka.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_BUF 1024

int main(int argc, char *argv[])
{
    int pd[2];
    int pid;
    char *izvorisni_fajl = argv[1];
    int brBajtova = atoi(argv[2]);
    char *odredisni_fajl = argv[3];
    
    if(argc < 3)
    {
        printf("Lose uneti argumenti. Uneti ih u obliku: %s <izvorisni_fajl> <br_bajtova> <odredisni_fajl>\n", argv[0]);
        exit(1);
    }
    
    if(pipe(pd) == -1)
    {
        printf("Greska prilikom kreiranja datavoda!\n");
        exit(1);
    }
    
    pid = fork();
    if(pid < 0)
    {
        printf("Greska prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    //Izvrsava se proces dete
    if(pid == 0)
    {
        //Proces dete upisuje u odredisni fajl
        close(pd[1]); //Zatvara se strana datavoda za pisanje
        
        int dest_fd = open(odredisni_fajl, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(dest_fd == -1)
        {
            perror("Otvori odredisni fajl!\n");
            exit(1);
        }
        
        char buffer[MAX_BUF];
        int bytes_read;
        while((bytes_read = read(pd[0], buffer, sizeof(buffer))) > 0)
        {
            if(write(dest_fd, buffer, bytes_read) != bytes_read)
            {
                perror("Upisi u odredisni fajl\n");
                exit(1);
            }
        }
        
        close(pd[0]);
        close(dest_fd);
        exit(1);
    }
    else
    {
        //Proces roditelj izvrsava ovaj deo
        //Proces roditelj cita iz izvorisnog fajla
        close(pd[0]);
        
        int src_fd = open(izvorisni_fajl, O_RDONLY);
        if(src_fd == -1)
        {
            perror("Otvori izvorisni fajl!\n");
            exit(1);
        }
        
        char buffer[brBajtova];
        int bytes_read;
        while((bytes_read = read(src_fd, buffer, brBajtova)) > 0) 
        {
            if(write(pd[1], buffer, bytes_read) != bytes_read)
            {
                perror("Upis u datavod!\n");
                exit(1);
            }
        }
        
        close(pd[1]);
        close(src_fd);
        wait(NULL);
    }
    return 0;
}
