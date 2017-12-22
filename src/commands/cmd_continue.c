#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "binary.h"
#include "breakpoints.h"
#include "commands.h"
#include "my-dbg.h"
#include "registers.h"

static int continue_execution(void)
{
    // TODO: handle breakpoint removal

    // TODO: handle continue signal
    if (ptrace(PTRACE_CONT, g_pid, NULL, NULL) == -1)
    {
        warn("ptrace failed");

        return 0;
    }

    return 1;
}

static void wait_program(void)
{
    int wstatus;

    if (waitpid(g_pid, &wstatus, 0) == -1)
    {
        warn("waitpid failed");

        return;
    }

    if (WIFSTOPPED(wstatus))
    {
        void *addr = (void *) get_register(MY_REG_RIP);

        if (is_breakpoint(addr))
        {
            struct my_bp *bp = get_breakpoint(addr);

            (void)bp;

            //TODO

            /* if (signal is SIGTRAP): return */
        }

        //TODO: message for signal
    }

    else
    {
        if (WIFEXITED(wstatus))
            printf("Process %d exited with code %hhd\n",
                   g_pid, WEXITSTATUS(wstatus));

        else if (WIFSIGNALED(wstatus))
            printf("Process %d terminated by signal %s\n",
                   g_pid, strsignal(WSTOPSIG(wstatus)));

        else
            warnx("something went wrong");

        g_quit = 1;
    }
}

static void cmd_continue(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    if (!continue_execution())
        return;

    puts("Continuing.");

    wait_program();
}

register_command(continue,
                 cmd_continue,
                 "Continue program execution",
                 "continue");
