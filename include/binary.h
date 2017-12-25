#ifndef BINARY_H
# define BINARY_H

# include <sys/types.h>

pid_t g_pid;

int g_signum;

void setup_binary(char **argv);

int wait_program(int step);

int single_step(void);

#endif /* BINARY_H */
