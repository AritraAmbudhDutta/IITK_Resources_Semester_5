#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int res = atoi(argv[argc-1]);
    res *= 2;
    if(argc == 2)
    {
        // if(strcmp(argv[1], "double") == 0)
        
        printf("%d", res);
        return 0;
    }
    char args[argc-1][10];

    char path[10];
    path[0] = '.';
    path[1] = '/';
    for(int i= 0; i < sizeof(argv[1]); i++)
        path[i+2] = argv[1][i];

    sprintf(argv[argc-1], "%d", res);
    if(execv(path, argv+1) == -1)
        printf("Unable to execute\n");
    return 0;

}