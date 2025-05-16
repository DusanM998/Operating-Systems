/*Linux program u kome korisnik sa tastature unosi recenicu. Zatim glavni program kreira jedan dodatni proces.
 * Ova dva procesa su povezani datavodima dvosmerno. Glavni proces reci unete recenice jednu po jednu (iza svake
 * poslate reci ceka odgovor procesa deteta pre nego posalje sledecu rec) datavodom salje drugom procesu.
 * Drugi proces u primljenim recima pronalazi sve cifre (koje se mogu naci bilo gde u reci, cak i na susednim pozicijama)
 * i prvom procesu koriscenjem datavoda vraca sumu pronadjenih cifara. Prvi proces kada primi taj broj od drugog procesa
 * na ekranu stampa: "Suma cifara u reci <rec> je <suma>" i zatim salje narednu rec.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

// Funkcija za izračunavanje sume cifara u reči
int calculate_digit_sum(const char *word) 
{
    int sum = 0;
    for (int i = 0; word[i] != '\0'; i++) 
    {
        if (isdigit(word[i])) 
        {
            sum += word[i] - '0'; // Pretvori karakter cifre u odgovarajuću vrednost
        }
    }
    return sum;
}

int main() 
{
    char sentence[MAX_SENTENCE_LENGTH];
    char word[MAX_WORD_LENGTH];
    int pipe_parent_to_child[2], pipe_child_to_parent[2];

    // Kreiranje datavoda
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) 
    {
        perror("Greska prilikom kreiranja datavoda!");
        exit(1);
    }

    // Kreiranje procesa deteta
    int pid = fork();
    if (pid < 0) 
    {
        perror("Greska prilikom kreiranja procesa!\n");
        exit(1);
    }

    if (pid == 0) 
    { 
        //Proces dete: u primljenim recima od roditelja pronalazi sve cifre
        //zatim vraca roditelju sumu cifara
        close(pipe_parent_to_child[1]); // Zatvori kraj za pisanje u prvom datavodu
        close(pipe_child_to_parent[0]); // Zatvori kraj za čitanje u drugom datavodu

        while (1) 
        {
            read(pipe_parent_to_child[0], word, MAX_WORD_LENGTH); // Čitaj reč od roditelja
            if (strcmp(word, "END") == 0) 
            {
                break; // Ako je reč "END", prekini
            }

            int digit_sum = calculate_digit_sum(word); // Izračunaj sumu cifara
            write(pipe_child_to_parent[1], &digit_sum, sizeof(digit_sum)); // Pošalji rezultat roditelju
        }

        close(pipe_parent_to_child[0]); // Zatvori preostale krajeve datavoda
        close(pipe_child_to_parent[1]);
        exit(0);
    } 
    else 
    { 
        //Proces roditelj: reci unete recenice datavodo salje procesu detetu
        //zatim kada dobije od procesa deteta cifre koje se nalaze u reci
        //vraca sumu datih cifara
        close(pipe_parent_to_child[0]); // Zatvori kraj za čitanje u prvom datavodu
        close(pipe_child_to_parent[1]); // Zatvori kraj za pisanje u drugom datavodu

        // Unos rečenice sa tastature
        printf("Unesite recenicu: ");
        fgets(sentence, MAX_SENTENCE_LENGTH, stdin);

        // Podela rečenice na reči i slanje svake reči detetu
        char *token = strtok(sentence, " \n");
        while (token != NULL) 
        {
            strcpy(word, token);
            write(pipe_parent_to_child[1], word, MAX_WORD_LENGTH); // Pošalji reč detetu

            int digit_sum;
            read(pipe_child_to_parent[0], &digit_sum, sizeof(digit_sum)); // Čitaj rezultat od deteta

            printf("Suma cifara u reci '%s' je %d\n", word, digit_sum);

            token = strtok(NULL, " \n"); // Idi na sledeću reč
        }

        // Pošalji signal detetu da je kraj ("END")
        strcpy(word, "END");
        write(pipe_parent_to_child[1], word, MAX_WORD_LENGTH);

        close(pipe_parent_to_child[1]); // Zatvori preostale krajeve datavoda
        close(pipe_child_to_parent[0]);
        wait(NULL); // Sačekaj da se dete završi
    }

    return 0;
}
