#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {
    long max_sems = _SC_SEM_NSEMS_MAX;
    if (max_sems == -1) {
        if (errno == EINVAL) {
            printf("The _SC_SEM_NSEMS_MAX option is not supported.\n");
        } else {
            printf("An error occurred while calling sysconf().\n");
        }
        return 1;
    }
    printf("The maximum number of semaphores a process can have is %ld.\n", max_sems);
    return 0;
}
