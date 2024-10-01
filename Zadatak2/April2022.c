/*Linux program koji kreira dodatni proces sa kojim komunicira dvosmerno koriscenjem datavoda.
 * Roditeljski proces generise 10 slucajnih brojeva u opsegu od 200 do 299 i salje ih putem datavoda procesu detetu.
 * Proces dete roditeljskom procesu koriscenjem datavoda vraca samo brojeve deljive sa 3 i to uvecane za 25.
 * Roditeljski proces tako primljene brojeve od procesa deteta treba da odstampa na ekranu.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_RANDOMS 10
#define MIN_VALUE 200
#define MAX_VALUE 299

int main() {
    int pipe_parent_to_child[2], pipe_child_to_parent[2];
    int random_numbers[NUM_RANDOMS];
    int result;

    // Kreiranje datavoda
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) 
    {
        printf("Doslo je do greske prilikom kreiranja datavoda!\n");
        exit(1);
    }

    // Kreiranje procesa deteta
    int pid = fork();
    if (pid < 0) 
    {
        printf("Doslo je do greske prilikom kreiranja procesa!\n");
        exit(1);
    }

    if (pid == 0) 
    { // Proces dete
        close(pipe_parent_to_child[1]); // Zatvori kraj za pisanje u prvom datavodu
        close(pipe_child_to_parent[0]); // Zatvori kraj za čitanje u drugom datavodu

        int num;
        while (read(pipe_parent_to_child[0], &num, sizeof(int)) > 0) 
        {
            if (num % 3 == 0) 
            {
                printf("Proces dete generisao: %d\n", num);
                int modified_num = num + 25;
                printf("Generisani broj %d + 25 = %d\n", num, modified_num);
                write(pipe_child_to_parent[1], &modified_num, sizeof(int));
            }
        }

        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);
        exit(0);
    } 
    else 
    { 
        // Proces roditelj
        close(pipe_parent_to_child[0]); // Zatvori kraj za čitanje u prvom datavodu
        close(pipe_child_to_parent[1]); // Zatvori kraj za pisanje u drugom datavodu

        srand(time(NULL)); // Inicijalizacija generatora slučajnih brojeva

        // Generisanje i slanje slučajnih brojeva
        for (int i = 0; i < NUM_RANDOMS; i++) 
        {
            random_numbers[i] = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
            write(pipe_parent_to_child[1], &random_numbers[i], sizeof(int));
        }

        close(pipe_parent_to_child[1]); // Zatvori kraj za pisanje nakon slanja svih brojeva

        // Primanje i ispisivanje brojeva od deteta
        while (read(pipe_child_to_parent[0], &result, sizeof(int)) > 0) 
        {
            printf("Proces roditelj primio od deteta broj: %d\n", result);
        }

        close(pipe_child_to_parent[0]); // Zatvori kraj za čitanje nakon prijema svih brojeva
        wait(NULL); // Čekanje da se proces dete završi
    }

    return 0;
}
