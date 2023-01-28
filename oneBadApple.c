/*
    name: Owen Gibson, JP Palacios
    date: January 27, 2023
    course: CIS 452-02 - Operating Systems Concepts

    description:

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 5

int main(){

    // User-input declarative space
    char *line = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
    char *argv[3];
    char *command, *argument;

    // process management declarative space

    int kNodes, status;

    //     // command syntax: > send_message [message] [node]"
    // printf("> send_message ");
    // fflush(stdout);
    // fgets(line, sizeof(line), stdin);

    // if(strcmp(line, "quit\n") == 0){
    //     exit(0);
    // }

    // // tokenize the line command
    // command  = strtok(line, " ");
    // argument = strtok(NULL, " ");

    // // include null character at the end of the argument
    // argument[strlen(argument) - 1] = '\0';

    // argv[0] = command;
    // argv[1] = argument;
    // argv[2] = NULL;

    // Prompt user for number of nodes to generate
    printf("Enter the number of nodes to generate: ");
    fflush(stdout);
    fgets(line, sizeof(line), stdin);
    kNodes = atoi(line);
    free(line);
    pid_t pid[kNodes];

    int badApple = 2;
    int apple = 0;

    // ask user what node to send it to and message

    // generate k number pipes

    // generate k number of nodes
    for (int i = 0; i < kNodes; i++){

        pid[i] = fork();

        if(pid[i] < 0){
            perror("Fork failed");
            exit(1);
        }

        if(pid[i] == 0){
            // read in the apple
            // read in the message after a small delay
            // check if I have the apple

            // if message was for me, verbose output
            // else say "next!" verbose output

            // check if i am the bad apple
            // write out the apple
            // write out the message

            printf("[%d] hello this is child %d\n", getpid(), i);

            exit(0);
        }
    }

    for (int i = 0; i < kNodes; i++){
        wait(&status);
    }

    return 0;
}
