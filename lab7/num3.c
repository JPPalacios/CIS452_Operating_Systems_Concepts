#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

int main(void){

    int semID, i = 1;

    while(1){
        semID = semget(IPC_PRIVATE, i, 00600);
        if (semID < 0){
            perror("semget: failed to create\n");
            break;
        }
        i++;
        if (semctl(semID, 0, IPC_RMID) < 0){
            perror("Unable to remove semaphore\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Max sem size: %d\n", i - 1);

    return 0;
}