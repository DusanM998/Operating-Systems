#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    double sum, avg, tmp;
    int i;
    
    printf("Broj argumenata komandne linije: %d\n", argc);
    
    sum = 0.0;
    avg = 0.0;
    
    //argc -> ukupan broj argumenata prosledjenih programu
    //brojac krece od 1 zato sto je 1. argument naziv programa
    for(int i=1; i<argc; i++)
    {
        tmp = atof(argv[i]); //atof -> ASCI to float (zato sto su argumenti prosledjeni kao niz karaktera)
        printf("Argument: %d: %s %f\n", i, argv[i], tmp);
        sum+= atof(argv[i]);
        printf("Parcijalna suma u iteraciji %d: %f\n", i, sum);
    }
    avg = sum / (double) (argc -1);
    printf("Suma argumenata komandne linije: %f\n",sum);
    printf("Srednja vrednost argumenata je: %f\n", avg);
    
    return 0;
}
