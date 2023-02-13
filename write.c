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
    // handling keyboard-interruption ctrl-c
    signal(SIGINT, keyboard_interrupt);

    // generate a unique key
    key_t key;
    call_status(key = ftok("./src/read.c", 65), "Unique key failed\n");

    // create shared memory segment
    int sharedMemoryID;
    call_status(sharedMemoryID = shmget(key, sizeof(messageType), SHARED_MEM_OPTIONS), "writer: unable to get shared memory\n");

    // attach this file to shared memory segment
    messageType *sharedMemoryPointer;
    call_status(((sharedMemoryPointer = shmat(sharedMemoryID, 0, 0)) == (void *) -1), "writer: Unable to attach\n");

    // writer goes first
    (*sharedMemoryPointer).turn = WRITER_TURN;

    while(1)
    {
        if ((*sharedMemoryPointer).turn == WRITER_TURN)
        {
            strcpy((*sharedMemoryPointer).message, get_user_input()) ;
            (*sharedMemoryPointer).turn = READER_TURN;
        }
    }

    // detach from shared memory segment
    call_status(shmdt(sharedMemoryPointer), "writer: unable to detach\n");

    // destroy shared memory segment
    call_status(shmctl(sharedMemoryID, IPC_RMID, 0), "writer: unable to deallocate\n");

    return 0;
}
