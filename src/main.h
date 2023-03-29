#ifndef MAIN_H
#define MAIN_H

#define BUFFER_SIZE 32

/* Program */

void launch_program(void);

void *run_baker_thread(void *arg);

/* Utilities */

int get_baker_value(void);

void logs(char *message);

int to_error_status(int value);

void check_status(char *message, int error_status);

#endif /* MAIN_H */
