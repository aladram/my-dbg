#include "errors.h"

#include <err.h>
#include <errno.h>

void ptrace_error(void)
{
    if (errno == ESRCH)
        err(1, "ptrace fatal error");

    warn("ptrace error");
}
