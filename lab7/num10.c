#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) == 0) {
        printf("Maximum number of open files (soft lim): %ld\n", limit.rlim_cur);
    } else {
        printf("Error retrieving system limit\n");
    }

    return 0;
}
