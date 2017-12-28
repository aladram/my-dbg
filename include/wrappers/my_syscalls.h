#ifndef MY_SYSCALLS_H
# define MY_SYSCALLS_H

# include <sys/ptrace.h>

long my_ptrace(enum __ptrace_request request,
               void *addr, void *data);

void my_wait(int *wstatus);

#endif /* MY_SYSCALLS_H */
