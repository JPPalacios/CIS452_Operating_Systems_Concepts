/*
    name: Owen Gibson, JP Palacios
    date: January 26, 2023
    course: CIS 452-02 - Operating Systems Concepts

    description: This program allows a child process to communicate
    with its parent the type of signal raised during execution. This implementation
    generates a random value to raise either SIGUSR1/SIGUSR2. Additionally, a SIGINT
    signal is monitored to gracefully exit the program.
    resources:

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>       // random seed
#include <stdlib.h>

// each signal has its own handler
void sigHandler0(int);
void sigHandler1(int);
void sigHandler2(int);

int main() {

    int fd[2];
    int pipeCreationResult;
    pid_t pid, childProcessID, parentProcessID;
    int status;

    parentProcessID = getpid();

    srand(time(NULL));   // Initialization, should only be called once.

    pipeCreationResult = pipe(fd);
    fflush(stdout); // clear anything stuck in buffer

    if(pipeCreationResult < 0){ // Pipe failed
        perror("Failed pipe creation\n");
        exit(1);
    }

    pid = fork();

    if(pid < 0){ // Fork failed
        perror("Fork failed");
        exit(1);
    }

    if(pid == 0){
        while(1){ // continously raise user-defined signals using kill(2)
            printf("waiting...\t");

            sleep(rand() % 6);

            if (rand() % 2 == 0) {
                kill(parentProcessID, SIGUSR1);
            }else{
                kill(parentProcessID, SIGUSR2);
            }
            fflush(stdout); // clear anything stuck in buffer
        }
    }else{
        // parent process
        printf("spawned child PID# %d\n", pid);

        fflush(stdout); // clear anything stuck in buffer

        while(1){ // continously register these signals
            signal(SIGINT,  sigHandler0);
            signal(SIGUSR1, sigHandler1);
            signal(SIGUSR2, sigHandler2);
        }
    }

    return 0;
}

void sigHandler0(int sigNum){
    printf(" received. That's it, I'm shutting you down...\n");
    exit(0);
}

void sigHandler1(int sigNum){
    printf("received a SIGUSR1 signal\n");
}

void sigHandler2(int sigNum){
    printf("received a SIGUSR2 signal\n");
}