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
    https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-times-get-process-child-process-times
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>

#define COMMAND_LENGTH 20
#define BUFFER_SIZE 32

int main ()
{
    pid_t pid, childProcessID, parentProcessID;
    int status, length;

    char *line = malloc(sizeof(char) * BUFFER_SIZE); // user input buffer
    char *argv[3];                                   // vector of pointers
    char *command, *argument;

    struct tms t;                                    // timer struct
    clock_t dub;
    int tics_per_second = sysconf(_SC_CLK_TCK);      // system-defined ticks per second

    while(1){

        printf("> ");
        fgets(line, sizeof(line), stdin);
        // printf("%s\n", line); // debug print, works so far

        if (strcmp(line, "quit\n") == 0){
            exit(0);
        }

        // tokenize the line using strtok
        command  = strtok(line, " ");
        argument = strtok(NULL, " ");
        // printf("%s\n%s\n", command, argument);  // debug print, works so far

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
            if (execvp(argv[0], &argv[0]) < 0) {
                perror("execution failed");
                exit(1);
            }

            exit(0);
        }
        else {
            // waiting until the child process terminates
            childProcessID = wait(&status);
            if(childProcessID < 0){
                puts("Wait() error.\n");
            }
            else if (!WIFEXITED(status))
                puts("Child did not exit successfully");
            else if ((dub = times(&t)) == -1)
                perror("times() error");
            else {
                printf("process was dubbed %ld cycles ago.\n\n", dub);
                printf("            utime           stime\n");
                printf("parent:    %ld        %ld\n", t.tms_utime, t.tms_stime);
                printf("child:     %ld        %ld\n", t.tms_cutime, t.tms_cstime);
            }
        }
    }   

    fflush(stdin);
    return 0;
}
