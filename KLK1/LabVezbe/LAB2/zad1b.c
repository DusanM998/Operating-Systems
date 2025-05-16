#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    double sum, avg, tmp;
    int i;
    char input[10];
    
    printf("Broj argumenata komandne linije: %d\n", argc);
    
    sum = 0.0;
    avg = 0.0;
    i=0;
    
    printf("Unesite realan broj ili KRAJ za kraj rada\n");
    scanf("%s", input);
    i++;
    
    while(strcmp(input, "KRAJ") != 0)
    {
        tmp = atof(input); //atof -> ASCI to float (zato sto su argumenti prosledjeni kao niz karaktera)
        printf("Argument: %d: %s %f\n", i, input, tmp);
        sum+= atof(input);
        printf("Parcijalna suma u iteraciji %d: %f\n", i, sum);
        
        scanf("%s", input);
        i++;
    }
    
    avg = sum / (double) (i -1);
    printf("Suma argumenata komandne linije: %f\n",sum);
    printf("Srednja vrednost argumenata je: %f\n", avg);
    
    return 0;
}
