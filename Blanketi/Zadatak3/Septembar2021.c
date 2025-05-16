/*Linux program koji omogucava da dva procesa komuniciraju i sinhronizuju svoje izvrsavanje koriscenjem signala. Prvi proces
 * po slucajnom principu kreira 1024 celobrojne vrednosti i smesta ih u binarnu datoteku prenos.dat. Kada izgenerise brojeve,
 * obavestava o tome drugi proces, slanjem signala SIGUSR1 i pauzira svoje izvrsenje. Kada drugi proces primi signal on 
 * sadrzaj datoteke prenos.dat prikazuje na standardni izlaz, salje prvom procesu signal SIGUSR2 i nakon toga pauzira svoje
 * izvrsavanje. Po prijemu signala SIGUSR2, prvi proces ponovo startuje citav postupak. Postupak se ponavlja 4096 puta.
 * Nakon toga se oba procesa zavrsavaju*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#define BROJ_ITERACIJA 4096
#define BROJ_VREDNOSTI 1024
#define DATOTEKA "prenos.dat"

// Globalne promenljive za PID-ove procesa
pid_t pid1, pid2;
int nastavi = 0; // Flag koji će signalizirati nastavak izvršenja procesa 1

// Funkcija za rukovanje signalom SIGUSR1 (proces 2)
void sigusr1_handler(int sig) {
    // Otvaranje binarne datoteke za čitanje
    int fd = open(DATOTEKA, O_RDONLY);
    if (fd < 0) {
        perror("Greska pri otvaranju datoteke za citanje");
        exit(1);
    }

    // Čitanje i ispisivanje generisanih brojeva
    int vrednosti[BROJ_VREDNOSTI];
    read(fd, vrednosti, sizeof(vrednosti));
    close(fd);

    printf("Proces 2: Prikaz brojeva iz datoteke:\n");
    for (int i = 0; i < BROJ_VREDNOSTI; i++) {
        printf("%d ", vrednosti[i]);
    }
    printf("\n");

    // Slanje signala SIGUSR2 procesu 1
    kill(pid1, SIGUSR2);
}

// Funkcija za rukovanje signalom SIGUSR2 (proces 1)
void sigusr2_handler(int sig) {
    // Postavljanje zastavice da bi se proces 1 nastavio
    nastavi = 1;
}

int main() {
    pid1 = getpid();

    // Kreiranje drugog procesa
    pid2 = fork();

    if (pid2 == 0) {  // Proces dete (proces 2)
        // Podesavanje obrade signala SIGUSR1
        signal(SIGUSR1, sigusr1_handler);

        // Čekanje na signal
        while (1) {
            pause();  // Proces 2 pauzira dok ne primi signal
        }

    } else {  // Proces roditelj (proces 1)
        // Podesavanje obrade signala SIGUSR2
        signal(SIGUSR2, sigusr2_handler);

        srand(time(NULL));  // Inicijalizacija generatora nasumičnih brojeva

        for (int iteracija = 0; iteracija < BROJ_ITERACIJA; iteracija++) {
            // Generisanje 1024 nasumična broja
            int vrednosti[BROJ_VREDNOSTI];
            for (int i = 0; i < BROJ_VREDNOSTI; i++) {
                vrednosti[i] = rand() % 1000;  // Nasumični broj od 0 do 999
            }

            // Pisanje brojeva u binarnu datoteku
            int fd = open(DATOTEKA, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Greska pri otvaranju datoteke za pisanje");
                exit(1);
            }
            write(fd, vrednosti, sizeof(vrednosti));
            close(fd);

            // Slanje signala SIGUSR1 procesu 2
            kill(pid2, SIGUSR1);

            // Pauziranje dok ne primi signal SIGUSR2
            nastavi = 0;
            while (!nastavi) {
                pause();
            }
        }

        // Nakon 4096 iteracija, oba procesa završavaju
        kill(pid2, SIGKILL);  // Prekida se drugi proces
    }

    return 0;
}
