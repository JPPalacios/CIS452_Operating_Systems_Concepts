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
    printf("Running writer program...\n");

    // generate a unique key
    key_t key;
    call_status(key = ftok("./src/read.c", 65), "Unique key failed\n");

    call_status(sharedMemoryID = shmget(key, MEM_BYTE_SIZE, SHARED_MEM_OPTIONS), "writer: unable to get shared memory\n");

    call_status(((sharedMemoryPointer = shmat(sharedMemoryID, 0, 0)) == (void *) -1), "writer: Unable to attach\n");

    // create a turn variable in shared memory
    int *turn = (int *) sharedMemoryPointer;

    // writer goes first
    *turn = WRITER_TURN;

    while(1)
    {

        if (*turn == WRITER_TURN)
        {
            get_user_input(sharedMemoryPointer);
            *turn = READER_TURN;
        }
    }

    // functions should be called when shutting down the program
    call_status(shmdt(sharedMemoryPointer), "writer: unable to detach\n");

    call_status(shmctl(sharedMemoryID, IPC_RMID, 0), "writer: unable to deallocate\n");

    return 0;
}
