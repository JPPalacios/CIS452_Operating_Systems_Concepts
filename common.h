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

#define SHARED_MEM_OPTIONS (IPC_CREAT | S_IRUSR | S_IWUSR)

void get_user_input(char *message);

void signal_handler(void);

void call_handler(int function_call, char *message);

#endif // COMMON_H_
