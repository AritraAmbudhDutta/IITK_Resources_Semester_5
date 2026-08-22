#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
int main(void)
{
   int x;
   printf("Enter: ");
   scanf("%d", &x);
   // int fd = 2;
   // close(fd);
   fprintf(stderr, "x=%d\n", x);
   //printf("x=%d\n", x);
   exit(0);
}
