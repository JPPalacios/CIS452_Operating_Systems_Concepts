#include <stdio.h>
#include <sys/resource.h>
#include <time.h>

int main() {

    printf("System Clock res in ms: %ld\n", CLOCKS_PER_SEC / 1000);

    return 0;
}
