#include "syscalls.h"

#include <stddef.h>
#include <string.h>

#include "exceptions.h"
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

#include "syscalls_list.h"

#define MY_CONCAT(_1, _2) _1 ## _2

#define CASE_SYSCALL(Syscall) if (syscall == MY_CONCAT(SYS_, Syscall)) return #Syscall

char *syscall_name(int syscall)
{
    if (syscall != -1)
{
CASES_SYSCALL
}

    throw(SyscallException);

    return NULL;
}

#undef CASE_SYSCALL
#define CASE_SYSCALL(Syscall) if (!strcmp(syscall, #Syscall)) return MY_CONCAT(SYS_, Syscall)

static int syscall_num_aux(char *syscall)
{
CASES_SYSCALL

    throw(SyscallException);

    return -1;
}

int syscall_num(char *syscall)
{
    int ret = syscall_num_aux(syscall);

    if (ret == -1)
        throw(SyscallException);

    return ret;
}
