#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16

int main(int argc, char *argv[])
{
    int status;
    long int i, loop, temp, *sharedMemoryPointer;
    int sharedMemoryID;
    pid_t pid;

    loop = atoi(argv[1]);

    int semID;
    struct sembuf p = { 0, -1, SEM_UNDO};
    struct sembuf v = { 0, +1, SEM_UNDO};

    sharedMemoryID = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(sharedMemoryID < 0)
    {
        perror("Unable to obtain shared memory\n");
        exit(EXIT_FAILURE);
    }

    sharedMemoryPointer = shmat(sharedMemoryID, 0, 0);

    if(sharedMemoryPointer == (void *) -1)
    {
        perror ("Unable to attach\n");
        exit (1);
    }

    /* create a new semaphore set for use by parent and child processes */
    semID = semget(IPC_PRIVATE, 1, 00600);
    if (semID < 0)
    {
        perror("semget: failed to create\n");
        exit(EXIT_FAILURE);
    }

    /* initialize the semaphore set referenced by the previosly obtained semID handle */
    if (semctl(semID, 0, SETVAL, 1) < 0)
    {
        perror("semctl: initialiation failure\n");
        exit(EXIT_FAILURE);
    }

    sharedMemoryPointer[0] = 0;
    sharedMemoryPointer[1] = 1;

    pid = fork();

    if(pid < 0)
    {
        printf("Fork failed\n");
    }

    if(pid == 0)
    {
        // Child

        for(i = 0; i < loop; i++)
        {

            // sem wait
            if (semop(semID, &p, 1) < 0)
            {
                perror("semop: wait failed\n");
                exit(EXIT_FAILURE);
            }

            // critical section
            // swap the contents of sharedMemoryPointer[0] and sharedMemoryPointer[1]
            temp = sharedMemoryPointer[0];
            sharedMemoryPointer[0] = sharedMemoryPointer[1];
            sharedMemoryPointer[1] = temp;

            // sem signal
            if (semop(semID, &v, 1) < 0)
            {
                perror("semop: wait failed\n");
                exit(EXIT_FAILURE);
            }

        }

        if(shmdt(sharedMemoryPointer) < 0)
        {
            perror ("Unable to detach\n");
            exit (1);
        }

        exit(0);
    }
    else
    {
        for(i = 0; i < loop; i++)
        {

            // sem wait
            if (semop(semID, &p, 1) < 0)
            {
                perror("semop: wait failed\n");
                exit(EXIT_FAILURE);
            }

            // critical section
            // swap the contents of sharedMemoryPointer[0] and sharedMemoryPointer[1]
            temp = sharedMemoryPointer[1];
            sharedMemoryPointer[1] = sharedMemoryPointer[0];
            sharedMemoryPointer[0] = temp;

            // sem signal
            if (semop(semID, &v, 1) < 0)
            {
                perror("semop: wait failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    wait(&status);

    printf("Values: %li\t%li\n", sharedMemoryPointer[0], sharedMemoryPointer[1]);

    /* remove the semaphore referenced by semID */
    if (semctl(semID, 0, IPC_RMID) < 0)
    {
        perror("Unable to remove semaphore\n");
        exit(EXIT_FAILURE);
    }

    if(shmdt(sharedMemoryPointer) < 0)
    {
        perror("Unable to detach\n");
        exit(EXIT_FAILURE);
    }

    if(shmctl(sharedMemoryID, IPC_RMID, 0) < 0)
    {
        perror("Unable to deallocate\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
