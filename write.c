#include <stdlib.h>
#include <stdio.h>

#include <signal.h> // signal handler
#include <unistd.h>

#include <sys/types.h> // system calls
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

// resource: https://www.geeksforgeeks.org/ipc-shared-memory/

int main(void)
{
    int sharedMemoryID;
    char *sharedMemoryPointer;

    // handling keyboard-interruption ctrl-c
    signal(SIGINT, signal_handler);
    printf("Running writer program...\n");

    // generate a unique key
    key_t key;

    call_handler(key = ftok("./src/read.c", 65), "Unique key failed\n");

    call_handler(sharedMemoryID = shmget(key, MEM_BYTE_SIZE, SHARED_MEM_OPTIONS), "writer: unable to get shared memory\n");

    call_handler(((sharedMemoryPointer = shmat(sharedMemoryID, 0, 0)) == (void *) -1), "writer: Unable to attach\n");

    // while(1)
    // {
    get_user_input(sharedMemoryPointer);
    // }

    call_handler(shmdt(sharedMemoryPointer), "writer: unable to detach\n");

    return 0;
}
