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

int semaphore_create()
{
    /* create a new semaphore set for use by parent and child processes */
    int semID = semget(IPC_PRIVATE, 1, 00600);
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

    return semID;
}

void semaphore_wait(int semaphoreID)
{
    struct sembuf p = { 0, -1, SEM_UNDO};

    if (semop(semaphoreID, &p, 1) < 0)
    {
        perror("semop: wait failed\n");
        exit(EXIT_FAILURE);
    }
}

void semaphore_signal(int semaphoreID)
{
    struct sembuf v = { 0, +1, SEM_UNDO};

    if (semop(semaphoreID, &v, 1) < 0)
    {
        perror("semop: wait failed\n");
        exit(EXIT_FAILURE);
    }
}

void semaphore_removal(int semaphoreID)
{
    if (semctl(semaphoreID, 0, IPC_RMID) < 0)
    {
        perror("Unable to remove semaphore\n");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    int status;
    long int i, loop, temp, *sharedMemoryPointer;
    int sharedMemoryID;
    pid_t pid;


    /* printing values out */
    printf("value of IRUSR: %o\n", S_IRUSR);
    printf("value of IWUSR: %o\n", S_IWUSR);
    printf("value of IXUSR: %o\n", S_IXUSR);
    printf("value of IRGRP: %o\n", S_IRGRP);
    printf("value of IWGRP: %o\n", S_IWGRP);
    printf("value of IXGRP: %o\n", S_IXGRP);
    printf("value of IROTH: %o\n", S_IROTH);
    printf("value of IWOTH: %o\n", S_IWOTH);
    printf("value of IXOTH: %o\n", S_IXOTH);

    printf("value of S_IWUSR | S_IRUSR: %X\n", S_IWUSR | S_IRUSR);


    /* controlled process synch */

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

    int semID = semaphore_create();

    sharedMemoryPointer[0] = 10;
    sharedMemoryPointer[1] = 11;

    pid = fork();

    if(pid < 0)
    {
        printf("Fork failed\n");
    }

    if(pid == 0)
    {
        /* wait our turn */
        semaphore_wait(semID);

        /* critical section */
        temp = sharedMemoryPointer[0];
        sharedMemoryPointer[0] = sharedMemoryPointer[1];
        sharedMemoryPointer[1] = temp;

        /* signal our turn's over */
        semaphore_signal(semID);

        if(shmdt(sharedMemoryPointer) < 0)
        {
            perror ("Unable to detach\n");
            exit (1);
        }

        exit(0);
    }
    else
    {
        /* wait our turn */
        semaphore_wait(semID);

        /* critical section */
        temp = sharedMemoryPointer[0];
        sharedMemoryPointer[0] = sharedMemoryPointer[1];
        sharedMemoryPointer[1] = temp;

        /* signal our turn's over */
        semaphore_signal(semID);
    }

    wait(&status);

    printf("Values: %li\t%li\n", sharedMemoryPointer[0], sharedMemoryPointer[1]);

    semaphore_removal(semID);

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
