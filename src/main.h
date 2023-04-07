#ifndef MAIN_H
#define MAIN_H

#define BUFFER_SIZE 32

#define MIXER_COUNT  2
#define PANTRY_COUNT 1
#define FRIDGE_COUNT 2
#define BOWL_COUNT   3
#define SPOON_COUNT  5
#define OVEN_COUNT   1

#define TOTAL_RECIPE_COUNT 5

#define PANTRY_DELAY_U 100
#define FRIDGE_DELAY_U 100
#define MIX_DELAY_U    100
#define BAKE_DELAY_U   100

#define SETUP 1
#define DESTROY 0

sem_t mixer;
sem_t pantry;
sem_t fridge;
sem_t bowl;
sem_t spoon;
sem_t oven;

int valp, ramsey, ramsey_recipe;

char *recipe_book[TOTAL_RECIPE_COUNT] = {"cookies", "pancakes", "pizza dough", "pretzels", "cinnamon rolls"};

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
