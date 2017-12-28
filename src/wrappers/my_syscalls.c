#include "my_syscalls.h"

#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "binary.h"
#include "exceptions.h"

long my_ptrace(enum __ptrace_request request,
               void *addr, void *data)
{
    errno = 0;

    long ret = ptrace(request, g_pid, addr, data);

    if (errno)
        throw(PtraceException);

    return ret;
}

void my_wait(int *wstatus)
{
    errno = 0;

    waitpid(g_pid, wstatus, 0);

    if (errno)
        throw(WaitException);;
}
