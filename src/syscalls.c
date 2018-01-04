#include "syscalls.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "breakpoints.h"
#include "exceptions.h"
#include "memory_utils.h"
#include "my_syscalls.h"
#include "registers.h"

int *g_syscalls;

size_t g_syscalls_nb;

int is_syscall()
{
    size_t data = my_ptrace(PTRACE_PEEKDATA,
                            (void *) (get_register(MY_REG_RIP) - 2),
                            NULL);

    return (data & 0xFFFF) == 0x050F;
}

int in_syscalls(int syscall)
{
    if (syscall == -1)
        return 0;

    for (size_t i = 0; i < g_syscalls_nb; ++i)
        if (syscall == g_syscalls[i])
            return 1;

    return 0;
}

void add_syscall_break(int syscall)
{
    g_syscalls = my_realloc(g_syscalls, sizeof(int) * ++g_syscalls_nb);

    g_syscalls[g_syscalls_nb - 1] = syscall;

    place_breakpoint((void *) (uintptr_t) syscall, MY_BP_SYSCALL);
}

#include "syscalls_list.h"

#define MY_CONCAT(_1, _2) _1 ## _2

#define CASE_SYSCALL(Syscall) if (syscall == MY_CONCAT(SYS_, Syscall)) \
                                  return #Syscall

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
#define CASE_SYSCALL(Syscall) if (!strcmp(syscall, #Syscall)) \
                                  return MY_CONCAT(SYS_, Syscall)

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
