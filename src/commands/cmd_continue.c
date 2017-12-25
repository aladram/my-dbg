#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/ptrace.h>

#include "binary.h"
#include "breakpoints.h"
#include "commands.h"
#include "my-dbg.h"
#include "registers.h"

static void continue_execution(void)
{
    void *addr = (void *) get_register(MY_REG_RIP);

    if (is_breakpoint(addr) && !single_step())
        return;

    if (ptrace(PTRACE_CONT, g_pid, NULL, g_signum) == -1)
        goto error;

    g_signum = 0;

    wait_program(0);

    return;

error:
    warn("ptrace failed");
}

static void cmd_continue(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    puts("Continuing.");

    continue_execution();
}

register_command(continue,
                 cmd_continue,
                 "Continue program execution",
                 "continue");
