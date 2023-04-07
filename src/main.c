#include <stdbool.h>
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

int main(void)
{
    // logs("launching program...");

    signal(SIGINT, keyboard_interrupt);

    control_semaphores(SETUP);

    create_bakers();

    control_semaphores(DESTROY);

    return 0;
}

void control_semaphores(int setup)
{
    if (setup == 1)
    {
        logs("setting up semaphores");

        sem_init(&mixer,  0, MIXER_COUNT);
        sem_init(&pantry, 0, PANTRY_COUNT);
        sem_init(&fridge, 0, FRIDGE_COUNT);
        sem_init(&bowl,   0, BOWL_COUNT);
        sem_init(&spoon,  0, SPOON_COUNT);
        sem_init(&oven,   0, OVEN_COUNT);
    }
    else
    {
        logs("destroying semaphores");

        sem_destroy(&mixer);
        sem_destroy(&pantry);
        sem_destroy(&fridge);
        sem_destroy(&bowl);
        sem_destroy(&spoon);
        sem_destroy(&oven);
    }

}

void create_bakers(void)
{
    int status;
    int nBakers = get_baker_value();
    pthread_t baker_thread[nBakers];
    int baker_ID[nBakers];

    // logs("bakers getting ready...");
    for (int i = 0; i < nBakers; i++) {
        baker_ID[i] = i;
    }

    for (int i = 0; i < nBakers; i++)
    {
        status = pthread_create(&baker_thread[i], NULL, run_baker_thread, (void *) &baker_ID[i]);
        check_status("pthread_create() error", status);
    }

    for (int i = 0; i < nBakers; i++)
    {
        status = pthread_join(baker_thread[i], NULL);
        check_status("pthread_join() error", status);
    }

    logs("all bakers finished!");
}

void get_pantry_ingredients(int recipe, int baker)
{
    // printf("baker %d waiting to get pantry ingredients...\n", baker);

    sem_wait(&pantry);

    // switch (recipe) {
    //     case 0:
    //         printf("baker %d got flour and sugar from the pantry\n", baker);
    //         break;
    //     case 1:
    //         printf("baker %d got flour, sugar, baking soda, and salt from the pantry\n", baker);
    //         break;
    //     case 2:
    //         printf("baker %d got yeast, sugar, and salt from the pantry\n", baker);
    //         break;
    //     case 3:
    //         printf("baker %d got flour, sugar, salt, yeast, and baking soda from the pantry\n", baker);
    //         break;
    //     case 4:
    //         printf("baker %d got flour, sugar, salt, and cinnamon from the pantry\n", baker);
    //         break;
    //     default:
    //         printf("baker %d did not get any ingredients from the pantry\n", baker);
    // }

    printf("baker %d got pantry ingredients.\n", baker);

    sem_post(&pantry);
}

void get_fridge_ingredients(int recipe, int baker)
{
 
    // printf("baker %d waiting to get fridge ingredients...\n", baker);

    sem_wait(&fridge);
    // printf("baker %d in fridge %d.\n\n", baker, selected_fridge);
    
    // switch (recipe) {
    //     case 0:
    //         printf("baker %d got milk and butter from the fridge\n", baker);
    //         break;
    //     case 1:
    //         printf("baker %d got egg, milk, and butter from the fridge\n", baker);
    //         break;
    //     case 2:
    //         printf("baker %d did not get any ingredients from the fridge\n", baker);
    //         break;
    //     case 3:
    //         printf("baker %d got eggs from the fridge\n", baker);
    //         break;
    //     case 4:
    //         printf("baker %d got butter and eggs from the fridge\n", baker);
    //         break;
    //     default:
    //         printf("baker %d did not get any ingredients from the fridge\n", baker);
    // }

    printf("baker %d got fridge ingredients.\n", baker);
    sem_post(&fridge);

}

void get_utensils(int baker)
{
    // printf("baker %d waiting to get utensils...\n", baker);
    sem_wait(&bowl);
    sem_wait(&mixer);
    sem_wait(&spoon);
    printf("baker %d got utensils.\n\n", baker);
    sem_post(&spoon);
    sem_post(&mixer);
    sem_post(&bowl);
}

void *run_baker_thread(void *arg)
{
    int baker = *((int *) arg);

    int current_recipe = 0;

    while(current_recipe < 1)
    {
        /* baker getting ingredients from pantry */
        get_pantry_ingredients(current_recipe, baker);

        /* baker getting ingredients from fridge */
        get_fridge_ingredients(current_recipe, baker);

        // /* baker getting cooking utensils */
        get_utensils(baker);

        /* baker cooking recipe in oven */
        // printf("baker %d waiting to use the oven...\n", baker);
        sem_wait(&oven);
        printf("baker %d using oven.\n\n", baker);
        sem_post(&oven);

        printf("\nbaker %d finished the %s recipe!\n\n", baker, recipe_book[current_recipe]);
        current_recipe++;
    }

    pthread_exit(NULL);
}

/* Utilities */

void keyboard_interrupt(int signalNumber)
{
    if (signalNumber == SIGINT)
    {
        logs("terminating program...");

        /* removes semaphore */
        control_semaphores(DESTROY);

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
        printf("\n>$ Enter the number of bakers: ");
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