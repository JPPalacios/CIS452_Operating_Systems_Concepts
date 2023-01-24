/*
    name: Owen Gibson, JP Palacios
    date: January 20, 2023
    course: CIS 452-02 - Operating Systems Concepts

    description: implements a simple command interpreter,
    using OS concepts like process creation, suspension, execution, and termination
    -- basic loop: read, parse, execute
    resources:
    https://stackoverflow.com/questions/28502305/writing-a-simple-shell-in-c-using-fork-execvp?noredirect=1&lq=1
    https://brennan.io/2015/01/16/write-a-shell-in-c/
    https://purdueusb.com/wiki/terminal
    https://man7.org/linux/man-pages/man2/getrusage.2.html
    https://www.delftstack.com/howto/c/getrusage-example-in-c/
*/

#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define COMMAND_LENGTH 20
#define BUFFER_SIZE 32

int main ()
{
    pid_t pid, childProcessID, parentProcessID;
    int status, length;
    struct rusage usage;
    long prevSwitch = 0;
    long nextSwitch = 0;

    char *line = malloc(sizeof(char) * BUFFER_SIZE); // user input buffer
    char *argv[3];                                   // vector of pointers
    char *command, *argument;

    while(1){

        printf("> ");
        fgets(line, sizeof(line), stdin);

        if (strcmp(line, "quit\n") == 0){
            exit(0);
        }

        // tokenize the line using strtok
        command  = strtok(line, " ");
        argument = strtok(NULL, " ");

        // change end character to null character for arguments
        length = strlen(argument);
        argument[length - 1] = '\0';

        // assigning our command and argument to vector of pointers
        argv[0] = command;
        argv[1] = argument;
        argv[2] = NULL;

        // spawn a child from the depths of hell to execute the souls of the living
        if ((pid = fork()) < 0) {
            fprintf(stderr, "Fork failure: %s", strerror(errno));
            exit(1);
        }
        else if (pid == 0) {
            // child process to execute the command execvp
            printf("\n");
            if (execvp(argv[0], &argv[0]) < 0) {
                perror("execution failed");
                exit(1);
            }

            exit(0);
        }
        else {
            // waiting until the child process terminates
            wait(&status);
        }

        getrusage(RUSAGE_CHILDREN, &usage);
        printf("\nuser CPU time used was %ld us\n\n", usage.ru_utime.tv_usec);

        nextSwitch = usage.ru_nivcsw;
        printf("involuntary contexts switches: %ld times\n\n", (nextSwitch - prevSwitch));
        prevSwitch = nextSwitch;

        fflush(stdin);
    }

    return 0;
}
