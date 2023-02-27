#include <stdio.h>
#include <unistd.h>

int main() {
    long maxsize = sysconf(_SC_FSIZE_MAX);

    if (maxsize != -1) {
        printf("Maximum file size: %ld bytes\n", maxsize);
    } else {
        printf("Error retrieving system limit\n");
    }

    return 0;
}
