#include "syscalls.h"

#include <stddef.h>

#include "memory_utils.h"
#include "my_syscalls.h"
#include "registers.h"

int *syscalls;

size_t syscalls_nb;

int is_syscall()
{
    size_t data = my_ptrace(PTRACE_PEEKDATA,
                            (void *) (get_register(MY_REG_RIP) - 2),
                            NULL);

    return (data & 0xFFFF) == 0x050F;
}

int in_syscalls(int syscall)
{
    for (size_t i = 0; i < syscalls_nb; ++i)
        if (syscall == syscalls[i])
            return 1;

    return 0;
}

void add_syscall_break(int syscall)
{
    syscalls = my_realloc(syscalls, sizeof(int) * ++syscalls_nb);

    syscalls[syscalls_nb - 1] = syscall;
}
