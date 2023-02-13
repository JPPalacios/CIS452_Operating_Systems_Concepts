#include <stdlib.h>
#include <stdio.h>

#include <signal.h> // signal handler
#include <unistd.h>

#include <sys/types.h> // system calls
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

void keyboard_interrupt(int signalNumber);
char *get_user_input(void);
void call_status(int functionCall, char *message);

messageType *sharedMemoryPointer;

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
    call_status(((sharedMemoryPointer = shmat(sharedMemoryID, 0, 0)) == (void *) -1), "writer: Unable to attach\n");

    // writer goes first
    (*sharedMemoryPointer).turn = WRITER_TURN;

    // cleared cleanup flag allows program to continue
    (*sharedMemoryPointer).cleanup = 0;

    while(!(*sharedMemoryPointer).cleanup)
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

char *get_user_input(void)
{
    char buffer[TEXT_SIZE];
    char *userInput = malloc(strlen(buffer) + 1);

    fputs("\n[writer] - enter a message: ", stdout);
    fgets(buffer, sizeof(buffer), stdin);

    // handling "quit" from user input
    call_status((strcmp(buffer, "quit\n") == 0) * -1, " ");
    buffer[strlen(buffer) - 1] = '\0';

    return strcpy(userInput, buffer);
}

void keyboard_interrupt(int signalNumber)
{
    printf(" received. Terminating program...\n");

    // cleanup flag set HIGH allows reader to clean up
    (*sharedMemoryPointer).cleanup = 1;

    // detach from shared memory segment
    call_status(shmdt(sharedMemoryPointer), "writer: unable to detach\n");

    exit(EXIT_SUCCESS);
}

void call_status(int functionCall, char *message)
{
    if (functionCall < 0)
    {
        if (strcmp(message, " ") == 0)
        {
            // cleanup flag set HIGH allows reader to clean up
            (*sharedMemoryPointer).cleanup = 1;
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror(message);
            exit(EXIT_FAILURE);
        }
    }
}
