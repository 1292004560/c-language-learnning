#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 10
int main(void)
{
    char  buffer[MAXLEN];

    while (fgets(buffer, MAXLEN, stdin) != NULL)
        if (fputs(buffer, stdout) == EOF)
            printf("output error !!!!");
    
    if (ferror(stdin)){
        printf("input error !!!");
    }
}