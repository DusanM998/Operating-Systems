/*Modifikovan zad 1, tako da su 3 procesa datavodima povezani u krug. Drugi proces dete ne stampa recenicu vec je datavodom salje procesu rodjitelju koji je onda stampa na std izlaz bez ikakve modifikacije.*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/types.h>

int main()
{
    int pd1[2];
    int pd2[2];
    int pd3[2];
    char linija[80];
    int pid1, pid2;
    
    if(pipe(pd1) == -1)
    {
        printf("Greska prilikom kreiranja 1. datavoda!\n");
        return -1;
    }
    
    if(pipe(pd2) == -1)
    {
        printf("Greska prilikom kreiranja 2. datavoda!\n");
        return -1;
    }
    
    if(pipe(pd3) == -1)
    {
        printf("Greska prilikom kreiranja 3. datavoda!\n");
        return -1;
    }
    
    if(pid1 = fork() != 0)
    {
        close(pd1[0]);
        close(pd2[0]);
        close(pd2[1]);
        close(pd3[1]);
        do
        {
            gets(linija);
            
            write(pd1[1], linija, strlen(linija) + 1);
            printf("Prvi proces poslao je u pd1: %s\n", linija);
            
            read(pd3[0], linija, 80);
            printf("Modifikovana recenica: %s\n", linija);
        }
        while(strcmp(linija, "KRAJ.") != 0);
        
        printf("Prvi proces primio KRAJ\n");
        wait(NULL);
        close(pd1[1]);
        printf("Prvi proces izlazi\n");
        return 0;
    }
    else if(pid2 = fork() !=0)
    {
        printf("Kreiran je drugi proces!\n");
        char linija1[80];
        close(pd1[1]);
        close(pd2[0]);
        close(pd3[0]);
        close(pd3[1]);
        
        do
        {
            read(pd1[0], linija1, 80);
            printf("Drugi proces primio: %s\n", linija1);
            
            linija1[0] = toupper(linija[0]);
            
            write(pd2[1], linija1, strlen(linija1) + 1);
            printf("Drugi proces poslao: %s\n", linija1);
        }
        while(strcmp(linija1, "KRAJ") != 0);
        
        printf("Drugi proces primio KRAJ\n");
        
        wait(NULL);
        
        close(pd1[0]);
        close(pd2[1]);
        
        printf("Drugi proces izlazi!\n");
        exit(0);
    }
    else
    {
        char linija2[80];
        
        printf("Kreiran je treci proces\n");
        close(pd1[0]);
        close(pd1[1]);
        close(pd2[1]);
        close(pd3[0]);
        
        do
        {
            read(pd2[0], linija2, 80);
            printf("Treci proces primio: %s\n", linija2);
            
            if(linija2[strlen(linija2)-1] != '.')
            {
                linija2[strlen(linija2)+1] = '\0';
                linija2[strlen(linija2)] = '.';
            }
            printf("Treci proces poslao: %s\n", linija2);
            write(pd3[1], linija2, strlen(linija2)+1);
        }
        while(strcmp(linija2, "KRAJ.") !=0);
        
        printf("Treci proces je primio KRAJ.");
        sleep(1);
        close(pd2[0]);
        close(pd3[1]);
        printf("Treci proces izlazi!\n");
        exit(0);
    }
}
