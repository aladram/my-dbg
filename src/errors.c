#include "errors.h"

#include <err.h>
#include <errno.h>

#include "binary.h"

void ptrace_error(void)
{
    if (errno == ESRCH && g_pid)
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

void printf_error(void)
{
    warnx("printf error");
}

void io_error(void)
{
    warnx("IO error");
}

void memory_error(void)
{
    warn("memory error");
}

void elf_error(void)
{
    warnx("elf error");
}
