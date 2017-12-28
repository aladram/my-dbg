#include "errors.h"

#include <err.h>
#include <errno.h>

void ptrace_error(void)
{
    if (errno == ESRCH)
        err(1, "ptrace fatal error");

    warn("ptrace error");
}

void wait_error(void)
{
    warn("wait error");
}

void sscanf_error(void)
{
    warnx("sscanf error");
}
