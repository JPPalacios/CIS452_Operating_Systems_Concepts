#include <stdlib.h>
#include <stdio.h>

#include <signal.h> // signal handler
#include <unistd.h>

#include <sys/types.h> // system calls
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"


int main(void)
{
    int sharedMemoryID;
    char *sharedMemoryPointer;

    // handling keyboard-interruption ctrl-c
    signal(SIGINT, signal_handler);
    printf("Running reader program...\n");

    // generate a unique key
    key_t key;

    call_handler(key = ftok("./src/read.c", 65), "Unique key failed\n");

    call_handler(sharedMemoryID = shmget(key, MEM_BYTE_SIZE, SHARED_MEM_OPTIONS), "reader: unable to get shared memory\n");

    call_handler(((sharedMemoryPointer = shmat(sharedMemoryID, 0, 0)) == (void *) -1), "reader: unable to attach\n");

    // printf("shared contents: %s\n", sharedMemoryPointer);
    while(strcmp(sharedMemoryPointer, "") == 0);
    printf("[reader] - message: %s\n", sharedMemoryPointer);

    call_handler(shmdt(sharedMemoryPointer), "reader: unable to detach\n");

    call_handler(shmctl(sharedMemoryID, IPC_RMID, 0), "reader: unable to deallocate\n");

    return 0;
}
