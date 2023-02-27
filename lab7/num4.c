#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <errno.h>

#define SHM_KEY 12345

int main() {
    int shmid, size;
    char *shm;

    key_t key;
    size = 1024 * 1024; // start with a 1MB segment



    while (1) {
        shmid = shmget(key, size, S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | IPC_CREAT);
        if (shmid == -1) {
            if (errno == ENOMEM) {
                printf("Maximum shared memory size is %d bytes\n", size);
                break;
            } else {
                printf("Error creating shared memory segment\n");
                printf("Error code: %d\n", errno);
                return 1;
            }
        }
        shm = shmat(shmid, NULL, 0);
        if (shm == (char *) -1) {
            printf("Error attaching shared memory segment\n");
            return 1;
        }
        size *= 2; // double the segment size and try again
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
