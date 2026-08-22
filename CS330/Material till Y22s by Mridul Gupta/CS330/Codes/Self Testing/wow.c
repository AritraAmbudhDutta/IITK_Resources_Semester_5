#include <stdio.h>

void initialize_array(void)
{
    int array[4];
    int ctr = 4;
    printf("arr: %p\nctr: %p\n", array, &ctr);
    do{
       ctr--;
       array[ctr] = 0;
    }
    while(ctr >= 0);
    printf("%d\n", array[3]);
}

int main()
{
    initialize_array();
    // // int x;
    // int array[4];
    // int ctr = 4;
    // // int ctr1 = 8;
    // printf("arr: %p\nctr: %p\n", array, &ctr);
    // printf("val: %d\n", *(array-1));
    return 0;
}