#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main()
{
    char *x = NULL;
    printf("Pointer value of x: %p\n", x);

    // x = (char *)mmap(NULL, 4096*1024,PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
    x = (char *)malloc(1024*4096);
    printf("Pointer value of x in parent: %p\n", x);
    strcpy(x, "Hello there");

    printf("Parent: %s\n", x);

    if(fork() == 0)
    {
        printf("Pointer value of x in child: %p\n", x);
        strcpy(x, "CS330");
        printf("Child: %s\n", x);
        printf("Child pid: %ld\n", getpid());
        exit(0);
    }

    wait(NULL);

    printf("Parent: %s\n", x);
    printf("Parent pid: %ld\n", getpid());
    return 0;
}