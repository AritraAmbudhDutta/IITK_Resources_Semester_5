#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    int i;
    pid_t pid;
    printf("Before calling fork mypid = %d\n", getpid());
    
    pid = fork();
    if(pid < 0){
         perror("fork");
         exit(-1);
    }
    
    if(!pid){ /*Child*/
          int x;
          scanf("%d", &x);
          printf("Value of x: %d\n", x);
         printf("%s: Before calling exec mypid = %d\n", __FILE__, getpid());
         if(execl("./2.out", "2.out", NULL))
                 perror("exec");
         exit(-1);
    }
    sleep(3);
   for(i=0; i<5; ++i){
       // sleep(1);
        printf("%s After calling exec mypid = %d\n", __FILE__, getpid());
   }
}
