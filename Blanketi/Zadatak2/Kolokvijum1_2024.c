/*Linux program koji kreira jedan dodatni proces. Proces roditelj i proces dete komuniciraju koriscenjem datavoda.
 * Proces roditelj sa tastature cita prirodne brojeve (proverava da li je uneti broj prirodan) i unete prirodne brojeve
 * koriscenjem datavoda salje procesu detetu. Proces unosenja prirodnih brojeva se prekida kada korisnik
 * sa tastature unese rec "KRAJ". Nakon zavrsetka procesa unosa brojeva proces dete procesu roditelju vraca broj
 * koliko od unetih prirodnih brojeva je prosto.*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int pipe_roditelj_detetu[2], pipe_dete_roditelju[2];
    int pid;
    int brojUnetih = 0;
    int brojProstih = 0;
    char unos[1024];
    int broj;
    
    if(pipe(pipe_roditelj_detetu) < 0 || pipe(pipe_dete_roditelju) < 0)
    {
        printf("Greska prilikom kreiranja datavoda!\n!");
        exit(1);
    }
    
    if((pid = fork()) < 0)
    {
        printf("Greska prilikom kreiranja procesa!\n");
        exit(1);
    }
    
    if(pid == 0)
    {
        close(pipe_roditelj_detetu[1]);
        close(pipe_dete_roditelju[0]);
        while(1)
        {
            read(pipe_roditelj_detetu[0], &broj, sizeof(broj));
            if(broj == -1)
            {
                printf("Unet je KRAJ. Prekid komunikacije!\n");
                break;
            }
        
            if(broj <= 1)
            {
                printf("Proces dete: %d broj nije prost!\n", broj);
            }
            else
            {
                int prost = 1;
                for(int i=2; i * i <= broj; i++)
                {
                    if(broj % i == 0)
                    {
                        prost = 0;
                        printf("Proces dete: %d broj nije prost!\n", broj);
                        break;
                    }
                }
                if(prost)
                {
                    printf("Proces dete: Primljen je prost broj: %d\n", broj);
                    brojProstih++;
                }
            }
            
        }
        write(pipe_dete_roditelju[1], &brojProstih, sizeof(brojProstih));
        
        close(pipe_roditelj_detetu[0]);
        close(pipe_dete_roditelju[1]);
        exit(0);
    }
    else
    {
        close(pipe_roditelj_detetu[0]);
        close(pipe_dete_roditelju[1]);
        
        printf("Unesite prirodan broj (ili KRAJ za prekid programa): ");
        
        while(1)
        {
            
            
            fgets(unos, sizeof(unos), stdin);
            unos[strcspn(unos, "\n")] = '\0';
            
            broj = atoi(unos);
            
            if(strcmp(unos, "KRAJ") == 0)
            {
                broj = -1;
                write(pipe_roditelj_detetu[1], &broj, sizeof(unos));
                break;
            }
            
            
            
            if(broj <= 0)
            {
                printf("Proces roditelj: Unos nije validan! Unet je broj: %d koji nije prirodan\n", broj);
                continue;
            }
            brojUnetih++;
            write(pipe_roditelj_detetu[1], &broj, sizeof(broj));
        }
        
        while(read(pipe_dete_roditelju[0], &brojProstih, sizeof(brojProstih)) > 0)
        {
            printf("Proces roditelj: Od procesa deteta dobio koliko od unetih prirodnih brojeva (%d) je prosto: %d\n", brojUnetih, brojProstih);
        }
        
        
        close(pipe_roditelj_detetu[1]);
        close(pipe_dete_roditelju[0]);
        
        wait(NULL);
    }
    
    return 0;
}
