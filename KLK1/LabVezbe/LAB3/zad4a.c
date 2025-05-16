#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int i;
    
    /*U petlji se stampaju svi ulazni argumenti programa*/
    for(i=0; i < argc; i++) //prvi argument je uvek naziv programa (u idealnoj situaciji je pitanja na kojoj je program pozvan na izvrsenje)
        printf("argv[%d] = %s\n/", i, argv[i]);
    return 0;
}
