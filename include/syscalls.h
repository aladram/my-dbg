#ifndef SYSCALLS_H
# define SYSCALLS_H

# include <stddef.h>

int *g_syscalls;

size_t g_syscalls_nb;

int is_syscall();

int in_syscalls(int syscall);

void add_syscall_break(int syscall);

char *syscall_name(int syscall);

int syscall_num(char *syscall);

#endif /* SYSCALLS_H */
