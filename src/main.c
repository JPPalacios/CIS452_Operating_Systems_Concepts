#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>

#include "main.h"

// need to implement semaphores, threads, and shared memory
sem_t semaphore;

#define SIZE 32

int main(void)
{

    logs("launching program...");
    sem_init(&semaphore, 0, 1);

    signal(SIGINT, keyboard_interrupt);

    create_bakers();

    // launch_program()

    sem_destroy(&semaphore);

    return 0;
}

void create_bakers(void)
{
    int status;
    int nBakers = get_baker_value();
    pthread_t baker_thread[nBakers];

    logs("creating our bakers...");
    for (int i = 0; i < nBakers; i++)
    {
        status = pthread_create(&baker_thread[i], NULL, run_baker_thread, (void *) &i);
        check_status("pthread_create() error", status);
    }

    for (int i = 0; i < nBakers; i++)
    {
        status = pthread_join(baker_thread[i], NULL);
        check_status("pthread_join() error", status);
    }

    logs("threads created");
}

void *run_baker_thread(void *arg)
{

    /* baker getting ingredients */
    printf("baker %d waiting to get ingredients...\n", *((int *) arg));
    sem_wait(&semaphore);
    // get_ingredients();
    sem_post(&semaphore);
    printf("baker %d got ingredients!\n", *((int *) arg));

    /* baker getting cooking utensils */
    printf("baker %d waiting to get utensils...\n", *((int *) arg));
    sem_wait(&semaphore);
    // get_utensils();
    sem_post(&semaphore);
    printf("baker %d got utensils!\n", *((int *) arg));

    /* baker cooking recepe in oven */
    printf("baker %d waiting to use the oven...\n", *((int *) arg));
    sem_wait(&semaphore);
    // use_oven();
    sem_post(&semaphore);
    printf("baker %d finished recepe!\n", *((int *) arg));

    pthread_exit(NULL);
}

/* Utilities */

void keyboard_interrupt(int signalNumber)
{
    if (signalNumber == SIGINT)
    {
        logs("terminating program...");

        /* removes semaphore */
        sem_destroy(&semaphore);

        exit(EXIT_SUCCESS);
    }
}

int get_baker_value(void)
{
    char *buffer = malloc(BUFFER_SIZE);
    int nBakers = 0;
    int user_error = 0;

    do
    {
        printf(">$ Enter the number of bakers: ");
        fgets(buffer, sizeof(buffer), stdin);

        user_error = 0;
        if (sscanf(buffer, "%d", &nBakers) != 1)
        {
            user_error = 1;
        }
    }
    while(user_error);

    return nBakers;
}

void logs(char *message)
{
    printf("\n[+] %s \n", message);
}

int to_error_status(int value)
{
    return (value < 0) * -1;
}

void check_status(char *message, int error_status)
{
    char prefix[] = "\n[-] ";
    char suffix[] = "\n";

    /* calculate the size of the buffer needed */
    int bufsize = sizeof(prefix) + sizeof(message) + sizeof(suffix) + 1;

    /* allocate a buffer of the required size */
    char status_message[bufsize];

    /* concatenate the strings using snprintf() */
    snprintf(status_message, bufsize, "%s%s%s", prefix, message, suffix);

    if (error_status < 0)
    {
        perror(status_message);
        exit(EXIT_FAILURE);
    }

    if (error_status > 0)
    {
        printf("%s: %s\n", status_message, strerror(error_status));
    }
}