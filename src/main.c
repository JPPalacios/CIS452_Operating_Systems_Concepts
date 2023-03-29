#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#include "main.h"

int main(void)
{

    logs("launching program...");

    launch_program();

    return 0;
}

void launch_program(void)
{
    int status;
    int nBakers = get_baker_value();
    pthread_t baker_thread[nBakers];

    logs("creating our bakers...");

    for (int i = 0; i < nBakers; i++)
    {
        status = pthread_create(&baker_thread[i], NULL, run_baker_thread, (void *) &i);
        check_status("pthread_create() error", status);

        status = pthread_join(baker_thread[i], NULL);
        check_status("pthread_join() error", status);
    }

    logs("threads created");
}

void *run_baker_thread(void *arg)
{
    printf("\n[baker %d] - hello this is my own pthread! \n", (*(int *)arg));

    pthread_exit(NULL);
}

/* Utilities */

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
    }while(user_error);

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

    // calculate the size of the buffer needed
    int bufsize = sizeof(prefix) + sizeof(message) + sizeof(suffix) + 1;

    // allocate a buffer of the required size
    char status_message[bufsize];

    // concatenate the strings using snprintf()
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