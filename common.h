#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <signal.h> // signal handler
#include <unistd.h>

#include <sys/types.h> // system calls
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEM_BYTE_SIZE 4096
#define TEXT_SIZE 100

#define WRITER_TURN 1
#define READER_TURN 2

#define SHARED_MEM_OPTIONS (IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

typedef struct messageType
{
    int turn;
    char message[TEXT_SIZE];
} messageType;

int value;

char *get_user_input(void);

void keyboard_interrupt(int signal_number);

void call_status(int function_call, char *message);

#endif // COMMON_H_
