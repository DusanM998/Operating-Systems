/*Linux program koji sinhronizuje sadrzaj dva direktorijuma. Putanje do direktorijuma koji se sinhronizuju zadaju se kao
 * ulazni argumenti programa. Program za svaku regularnu datoteku iz prvog direktorijuma proverava da li postoji odgovarajuca
 * datoteka u drugom direktorijumu (uporedjuje ime i velicinu datoteke). Ukoliko datoteka ne postoji, onda se ona kopira iz prvog
 * direktorijuma u drugi direktorijum. Napomena: Pretpostaviti da prvi direktorijum ne sadrzi poddirektorijume.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_PATH 1024

void processdir(char *dir1, char *dir2)
{
    DIR *dp1, *dp2;
    struct dirent *dirp1, *dirp2;
    struct stat statbuf1, statbuf2;
    char path1[MAX_PATH], path2[MAX_PATH];
    int found;
    
    //Otvara prvi direktorijum
    if((dp1 = opendir(dir1)) == NULL)
    {
        printf("Greska prilikom otvaranja prvog direktorijuma!\n");
        exit(-1);
    }
    
    //Obradjuje sve datoteke u 1. direktorijumu
    while((dirp1 = readdir(dp1)) != NULL)
    {
        //Preskace trenutni i roditeljski direktorijum
        if(strcmp(dirp1->d_name, ".") == 0 || strcmp(dirp1->d_name, "..") == 0)
            continue;
        
        //Kreira putanju do datoteke u 1. direktorijumu
        strcpy(path1, dir1);
        strcat(path1, "/");
        strcat(path1,dirp1->d_name);
        
        if(stat(path1, &statbuf1) == -1)
        {
            printf("Greska prilikom ocitavanja statusa zadate putanje!\n");
            continue;
        }
        
        //Ispituje da li su datoteke u prvom direktorijumu regularne
        if(S_ISREG(statbuf1.st_mode))
        {
            //Otvara drugi direktorijum i trazi datoteke koje su istog imena i velicine
            if((dp2 = opendir(dir2)) == NULL)
            {
                printf("Greska prilikom otvaranja drugog direktorijuma!\n");
                closedir(dp1);
                exit(-1);
            }
            
            found = 0; //Flag koji signalizira da li je datoteka pronadjena
            while((dirp2 = readdir(dp2)) != NULL)
            {
                //Preskace trenutni i roditeljski direktorijum
                if(strcmp(dirp2->d_name, ".") == 0 || strcmp(dirp2->d_name, "..") == 0)
                    continue;
                
                //Kreira putanju do datoteke u 2. direktorijumu
                strcpy(path2, dir2);
                strcat(path2, "/");
                strcat(path2,dirp2-> d_name);
                
                if(stat(path2, &statbuf2) < 0)
                {
                    printf("Greska prilikom ocitavanja statusa zadate putanje!\n");
                    continue;
                }
                
                //Poredi ime i velicinu datoteke iz 1. i 2. direktorijuma
                if(S_ISREG(statbuf2.st_mode) && strcmp(dirp1->d_name, dirp2->d_name) == 0 && statbuf1.st_size == statbuf2.st_size)
                {
                    printf("Datoteka: %s postoji u oba direktorijuma(%s & %s) | (velicna datoteke: %ld B)\n", dirp1->d_name, path1, path2, statbuf1.st_size);
                    found = 1;
                    break;
                }
            }
            
            closedir(dp2);
            
            if(!found)
            {
                printf("Datoteka: %s ne postoji u drugom direktorijumu! Kopiram je iz prvog...\n", dirp1->d_name);
                
                // Kreira putanju do kopirane datoteke u drugom direktorijumu
                snprintf(path2, sizeof(path2), "%s/%s", dir2, dirp1->d_name);

                // Otvoriti datoteku u prvom direktorijumu za čitanje
                int src_fd = open(path1, O_RDONLY);
                if (src_fd < 0)
                {
                    perror("Greska prilikom otvaranja izvornog fajla");
                    continue;
                }

                // Otvoriti ili kreirati datoteku u drugom direktorijumu za pisanje
                int dest_fd = open(path2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (dest_fd < 0)
                {
                    perror("Greska prilikom otvaranja odredisnog fajla");
                    close(src_fd);
                    continue;
                }

                // Kopirati sadržaj iz izvornog u odredišni fajl
                char buffer[4096];
                ssize_t bytesRead;
                while ((bytesRead = read(src_fd, buffer, sizeof(buffer))) > 0)
                {
                    if (write(dest_fd, buffer, bytesRead) != bytesRead)
                    {
                        perror("Greska prilikom pisanja u odredisni fajl");
                        break;
                    }
                }

                if (bytesRead < 0)
                {
                    perror("Greska prilikom citanja iz izvornog fajla");
                }

                // Zatvoriti fajlove
                close(src_fd);
                close(dest_fd);
            }
        }
    }
    
    closedir(dp1);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Pokrenuti program: %s <putanja_do_1._direktorijuma> <putanja_do_2._direktorijuma>\n", argv[0]);
        exit(-1);
    }
    
    processdir(argv[1], argv[2]);
    
    return 0;
}
