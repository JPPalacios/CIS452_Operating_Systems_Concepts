#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(void){

    printf("Memory Page Size: %d\n", get_phys_pages());

    return 0;
}