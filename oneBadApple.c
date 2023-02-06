#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INT_BUFFER_SIZE 3
#define CHAR_BUFFER_SIZE 50

#define RECEIVING_END 0
#define SENDING_END 1

// handles a raised signal to gracefully terminate the program
void sigHandler(int sigNum){
    printf(" received. That's it, I'm shutting you down...\n");
    exit(0);
}

// message "object" to be passed from child node to child node
typedef struct messageType{
    int  header;
    char message[CHAR_BUFFER_SIZE];
} messageType;

// returns the user's desired number of nodes
int getNodeNum(void){

    char *line = malloc(INT_BUFFER_SIZE);
    int k;

    printf("Enter desired number of communcation nodes: ");
    fflush(stdout);
    fgets(line, sizeof(line), stdin);
    k = atoi(line);
    free(line);

    return k;
}

// returns a message object to be passed from node 0 to node k and back to node 0
messageType getMessage(int kNodes){

    char *line = malloc(INT_BUFFER_SIZE);
    messageType newMessage;

    do{
        printf("Send to which node? (range: 0 to k - 1, enter \"q\" to quit): ");
        fgets(line, sizeof(line), stdin);

        if(strcmp(line, "q\n") == 0)
        {
            exit(0);
        }

        newMessage.header = atoi(line);
    }while(newMessage.header >= kNodes);

    printf("Message (must be under %d chars): ", CHAR_BUFFER_SIZE - 1);
    fgets(newMessage.message, sizeof(newMessage.message), stdin);
    newMessage.message[strcspn(newMessage.message, "\n")] = 0; //attempts to get rid of newline

    free(line);
    return newMessage;
}

int main(){

    srand(time(NULL));
    int kNodes = getNodeNum();
    pid_t pid[kNodes];
    int pipes[kNodes + 1][2];
    int badNode = (rand() % kNodes);

    while(1)
    {
        messageType apple = getMessage(kNodes);

        // generate k + 1 pipes
        for(int i = 0; i < kNodes + 1; i++)
        {
            if(pipe(pipes[i]) < 0)
            {
                perror("Pipe failed!");
                exit(1);
            }
        }

        // generate k nodes
        for(int currentNode = 0; currentNode < kNodes; currentNode++)
        {
            int nextNode = currentNode + 1;

            // create a child node and store pid
            pid[currentNode] = fork();

            if(pid[currentNode] < 0)
            {
                perror("Fork Failed!");
                exit(1);
            }

            // child node process scope
            if(pid[currentNode] == 0)
            {

                // close every non-adjacent pipe of the current child node
                for(int j = 0; j < kNodes; j++)
                {
                    if(j != currentNode)
                    {
                        close(pipes[j][RECEIVING_END]);
                    }
                    if(j != nextNode)
                    {
                        close(pipes[j][SENDING_END]);
                    }
                }

                // receive a message from k - 1 node
                messageType receivedApple;
                read(pipes[currentNode][RECEIVING_END], &receivedApple, sizeof(messageType));
                fflush(stdout);

                // check if current child node is the bad apple
                if(badNode == currentNode)
                {
                    printf("[node %2d] - This node is the bad apple, modifying message...\n",currentNode);
                    const char *badMessage = ", Your message got bad apple'd!";
                    strcat(receivedApple.message, badMessage);
                }

                //check if this message is for *this* child node
                if(receivedApple.header == currentNode)
                {
                    receivedApple.header = 0;
                    printf("[node %2d] - Message reached its destination!\n", currentNode);
                    printf("[node %2d] - Message reads: \"%s\"\n", currentNode, receivedApple.message);
                }
                else
                {
                    printf("[node %2d] - Message not addressed to this node.\n", currentNode);
                }


                // let the sender know it's the last child node in the network
                if (currentNode == kNodes - 1)
                {
                    printf("[node %2d] - Sending message to the last node...\n", currentNode);
                    fflush(stdout);
                }
                else
                {
                    printf("[node %2d] - Sending message to the next node...\n", currentNode);
                }

                // send the message to the next child node in line after some delay
                sleep(1);
                write(pipes[nextNode][SENDING_END], &receivedApple, sizeof(messageType));
                fflush(stdout);

                // task complete, close adjacent pipes and terminate current child node
                close(pipes[currentNode][RECEIVING_END]);
                close(pipes[nextNode][SENDING_END]);

                return 0;
            }
        }

        // install a signal to be raised when a user presses ctrl-c
        signal(SIGINT, sigHandler);

        // close non-adjacent pipes of the 0th child node
        for(int j = 0; j < kNodes; j++)
        {
            if(j != kNodes)
            {
                close(pipes[j][RECEIVING_END]);
            }
            if(j != 0)
            {
                close(pipes[j][SENDING_END]);
            }
        }

        // send the original message to the 0th child node
        printf("\nStarting new message transmission (%d nodes)...\n", kNodes);
        printf("\nSent message: \"%s\"\n\n", apple.message);
        write(pipes[0][SENDING_END], &apple, sizeof(messageType));

        // receive the message from kth child node
        read(pipes[kNodes][RECEIVING_END], &apple, sizeof(messageType));
        printf("\nReceived message: \"%s\"\n\n", apple.message);

        // transmissions complete, close remaining pipes
        printf("Message Transmission complete. Closing pipes...\n\n");
        close(pipes[0][RECEIVING_END]);
        close(pipes[kNodes][SENDING_END]);

        // wait for child processes to complete
        for (int i = 0; i < kNodes; i++)
        {
            wait(NULL);
        }

    }

    return 0;
}
