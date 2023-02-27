#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main() {
    long maxOpenFiles = sysconf(_SC_OPEN_MAX);

    if (maxOpenFiles != -1) {
        printf("Maximum number of open files (hard lim): %ld\n", maxOpenFiles);
    } else {
        printf("Error retrieving system limit\n");
    }

    return 0;
}
