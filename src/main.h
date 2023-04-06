#ifndef MAIN_H
#define MAIN_H

#define BUFFER_SIZE 32

#define MIXER_COUNT  2
#define FRIDGE_COUNT 2
#define BOWL_COUNT   3
#define SPOON_COUNT  5

#define TOTAL_RECIPE_COUNT 5

#define SETUP 1
#define DESTROY 0

sem_t mixer[MIXER_COUNT];
sem_t pantry;
sem_t fridge[FRIDGE_COUNT];
sem_t bowl[BOWL_COUNT];
sem_t spoon[SPOON_COUNT];
sem_t oven;

pthread_mutex_t mutex;

char *recipe_book[TOTAL_RECIPE_COUNT] = {"cookies", "pankcakes", "pizza dough", "pretzels", "cinnamon rolls"};

/* Program */

void control_semaphores(int setup);

void create_bakers(void);

void get_pantry_ingredients(int recipe, int baker);

void get_fridge_ingredients(int recipe, int baker);

void get_utensils(int baker);

void *run_baker_thread(void *arg);

/* Utilities */

void keyboard_interrupt(int signalNumber);

int get_baker_value(void);

void logs(char *message);

int to_error_status(int value);

void check_status(char *message, int error_status);

#endif /* MAIN_H */
