#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    for(int i = 0 ; i < 3; i++)
        if(i %2 == 0)
            fork();

    printf("%d\n", getpid());
    exit(0);
}