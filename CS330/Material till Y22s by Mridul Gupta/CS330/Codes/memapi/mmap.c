#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include <unistd.h>

int main()
{
	char *ptr;
	
	ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
    if(ptr == MAP_FAILED){
         perror("mmap");
         exit(-1);
	}
 
	// printf("ptr = %p\n %s", ptr, ptr);
	strcpy(ptr, "hello cs330!");
    printf("ptr = %p\n%s\n", ptr, ptr);
    munmap((void *)ptr, 4096);
    return 0;
}
