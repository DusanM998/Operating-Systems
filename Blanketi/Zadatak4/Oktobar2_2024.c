/*Linux program koji u zadatom direktorijumu i njegovim poddirektorijumima nalazi sve datoteke koje
 * imaju ekstenziju .sh i pokusava da ih izvrsi (koriscenjem sh komande). Putanja do direktorijuma 
 * koji se pretrazuje (apsolutna ili relativna) se prosledjuju programu kao argument komandne linije.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_NAME 1024

void pronadjiIizvrsiShDatoteke(char *folderName) 
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    int result;
    
    if ((dp = opendir(folderName)) == NULL) 
    {
        printf("Greska prilikom otvaranja foldera: %s!\n", folderName);
        return;
    }

    while ((dirp = readdir(dp)) != NULL) 
    {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        char tmp[MAX_NAME] = "";
        strcpy(tmp, folderName);
        strcat(tmp, "/");
        strcat(tmp, dirp->d_name);
        
        //snprintf(tmp, sizeof(tmp), "%s/%s", folderName, dirp->d_name);

        if ((result = stat(tmp, &statbuf)) == -1) 
        {
            printf("Neuspesno citanje podatka o objektu %s!\n", tmp);
            continue;
        }

        // Ako je regularna datoteka i ima ekstenziju .sh
        if (S_ISREG(statbuf.st_mode) && strstr(dirp->d_name, ".sh") != NULL) 
        {
            printf("Pronasao .sh datoteku: %s\n", tmp);
            printf("Pokrecem skriptu: %s\n", tmp);
            
            // Pokrećemo datoteku koristeći sh komandu
            int pid = fork();
            if (pid == 0) 
            {
                execlp("sh", "sh", tmp, NULL);
                perror("exec"); // Ako dođe do greške
                exit(EXIT_FAILURE);
            }
            else if (pid > 0) 
            {
                wait(NULL); // Čekamo da se skripta završi
            } 
            else 
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }

        // Ako je direktorijum, rekurzivno ga pretražujemo
        if (S_ISDIR(statbuf.st_mode)) 
        {
            printf("Pretrazujem folder: %s\n", tmp);
            pronadjiIizvrsiShDatoteke(tmp);
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) 
{
    if (argc != 2) 
    {
        printf("Koristite: %s <direktorijum>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pronadjiIizvrsiShDatoteke(argv[1]);
    
    return 0;
}
