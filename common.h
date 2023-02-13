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
#define TEXT_SIZE 256

#define BUFFER_SIZE 256

#define WRITER_TURN 1
#define READER_TURN 2

#define SHARED_MEM_OPTIONS (IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct messageType
{
    int cleanup;
    int turn;
    char message[TEXT_SIZE];
} messageType;

#endif // COMMON_H_
