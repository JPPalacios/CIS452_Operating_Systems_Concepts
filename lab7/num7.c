#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit limit;

    if (getrlimit(RLIMIT_NPROC, &limit) == 0) {
        printf("Maximum number of processes per user: %ld\n", limit.rlim_cur);
    } else {
        printf("Error retrieving resource limit\n");
    }

    return 0;
}
