/*
    name: Owen Gibson, JP Palacios
    date: January 27, 2023
    course: CIS 452-02 - Operating Systems Concepts

    description:

    pipes:
               output[0]  --------> input[1]
        close(output[0])  --- x --> close(input[1])

    TODO: fix piping issues (get random junk), parent needs to print stuff LAST,
    TODO: implement the signal stuff
    TODO: figure out how to execvp to run rusage info
    TOOO: make print statements about where stuff is going/coming from

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_BUFFER_SIZE 50
#define INT_BUFFER_SIZE 3

#define RECEIVING_END 0
#define SENDING_END 1

typedef struct messageType{
    int header;
    char message[CHAR_BUFFER_SIZE];
} messageType;

// Prompt user for number of nodes to generate
int getNodeNumber(void){

    char *line = calloc(INT_BUFFER_SIZE, sizeof(int));
    int k;

    printf("Enter the number of nodes to generate: ");
    fflush(stdout);
    fgets(line, sizeof(line), stdin);
    k = atoi(line);
    free(line);

    return k;
}

// ask user what node to send it to and message
messageType getNewMessage(void){

    char *line = calloc(INT_BUFFER_SIZE, sizeof(int));
    char *junk;
    messageType newMessage;

    printf("Send to: ");
    fgets(line, sizeof(line), stdin);
    newMessage.header = atoi(line);

    printf("Message: ");
    fgets(newMessage.message, sizeof(newMessage.message), stdin);
    newMessage.message[strcspn(newMessage.message, "\n")] = 0; // TODO: check this

    free(line);

    return newMessage;
}

int main(){

    int badNode = 1, status;

    // process management declarative space
    int kNodes = getNodeNumber();

    pid_t pid[kNodes];
    int pipes[kNodes + 1][2];

    messageType apple = getNewMessage();

    // strip the message of its '\n' character

    // generate k number pipes
    for (int i = 0; i < kNodes; i++)
    {
        if (pipe(pipes[i]) < 0){
            perror("Pipe failed!");
            exit(1);
        }
    }

    // generate k number of nodes
    for (int i = 0; i < kNodes; i++){

        pid[i] = fork();

        if(pid[i] < 0){
            perror("Fork failed!");
            exit(1);
        }

        if(pid[i] == 0){
            // read in the apple
            // read in the message after a small delay
            // check if I have the apple

            // close pipes that are not needed
            for (int j = 0; j < kNodes; j++){
                if(j != i){
                    close(pipes[j][RECEIVING_END]); // close output end to non-sequential nodes
                }
                if(j != i + 1){
                    close(pipes[j][SENDING_END]); // close input end to non-sequential nodes
                }
            }

            messageType receivedApple;

            read(pipes[i][RECEIVING_END], &receivedApple, sizeof(messageType));

            // check if i am the bad apple
            if (badNode == i){
                const char *badMessage = " bad message";
                strcat(receivedApple.message, badMessage);
            }

            if (receivedApple.header == i){
                printf("node [%d] - message received!\n", i);
                receivedApple.header = 0;
                printf("sending message to next node...\n");
                sleep(1);
            }else{
                printf("node [%d] - wrong node...\n", i);
            }

            write(pipes[i + 1][SENDING_END], &receivedApple, sizeof(messageType));

            close(pipes[i][RECEIVING_END]);
            close(pipes[i + 1][SENDING_END]);

            return 0;
        }
    }

    // Parent process

    // close pipes that are not needed
    for (int j = 0; j < kNodes; j++){
        if(j != kNodes){
            close(pipes[j][RECEIVING_END]);
        }
        if(j != 0){
            close(pipes[j][SENDING_END]);
        }
    }

    // writing to the 1st node in the ring
    write(pipes[0][SENDING_END], &apple, sizeof(messageType));

    // reading from the kth node in the ring
    read(pipes[kNodes][RECEIVING_END], &apple, sizeof(messageType));

    printf("received apple header: %d\n", apple.header);
    // printf("received apple message: %s\n", apple.message);

    for (int i = 0; i < strlen(apple.message); i++){
        printf("%c ", apple.message[i]);
    }

    for (int i = 0; i < kNodes; i++){
        wait(&status);
    }

    return 0;
}

    // User-input declarative space
    // char *argv[3];
    // char *command, *argument;

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
