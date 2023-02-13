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
    signal(SIGINT, keyboard_interrupt);
    printf("Running reader program...\n");

    // generate a unique key
    key_t key;
    call_status(key = ftok("./src/read.c", 65), "Unique key failed\n");

    call_status(sharedMemoryID = shmget(key, MEM_BYTE_SIZE, SHARED_MEM_OPTIONS), "reader: unable to get shared memory\n");

    call_status(((sharedMemoryPointer = shmat(sharedMemoryID, 0, 0)) == (void *) -1), "reader: unable to attach\n");

    int *turn = (int *) sharedMemoryPointer;

    while(1)
    {
        if(*turn == READER_TURN)
        {
            printf("[reader] - message: %s\n", sharedMemoryPointer + sizeof(int));
            *turn = WRITER_TURN;
        }
    }

    // functions should be called when shutting down the program
    call_status(shmdt(sharedMemoryPointer), "reader: unable to detach\n");

    call_status(shmctl(sharedMemoryID, IPC_RMID, 0), "reader: unable to deallocate\n");

    return 0;
}
