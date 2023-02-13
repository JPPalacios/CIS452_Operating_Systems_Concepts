#include <stdlib.h>
#include <stdio.h>

#include <signal.h> // signal handler
#include <unistd.h>

#include <sys/types.h> // system calls
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

#define BUFFER_SIZE 256
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// resource: https://www.geeksforgeeks.org/ipc-shared-memory/

extern int value;

char *get_user_input(void)
{
    char buffer[TEXT_SIZE];
    char *userInput = malloc(strlen(buffer) + 1);

    fputs("\nenter a message: ", stdout);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer) - 1] = '\0';

    // handling "quit" from user input
    call_status((strcmp(buffer, "quit\n") == 0) * -1, " ");

    return strcpy(userInput, buffer);;
}

void keyboard_interrupt(int signal_number)
{
    printf("\nexit %d\n", signal_number);

    value = 0;

    exit(EXIT_SUCCESS);
}

void call_status(int function_call, char *message)
{
    if (function_call < 0)
    {
        if (strcmp(message, " ") == 0)
        {
            value = 0;
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror(message);
            exit(EXIT_FAILURE);
        }
    }
}
