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

static int wait_program(void)
{
    int wstatus;

    if (waitpid(g_pid, &wstatus, 0) == -1)
    {
        warn("waitpid failed");

        return 0;
    }

    if (WIFSTOPPED(wstatus))
    {
        void *addr = (void *) (get_register(MY_REG_RIP) - 1);

        if (is_breakpoint(addr))
        {
            struct my_bp *bp = get_breakpoint(addr);

            (void)bp;

            //TODO

            /* if (signal is SIGTRAP): return */

            return 0;
        }

        //TODO: message for signal

        return 1;
    }

    if (WIFEXITED(wstatus))
        printf("Process %d exited with code %hhd\n",
                g_pid, WEXITSTATUS(wstatus));

    else if (WIFSIGNALED(wstatus))
        printf("Process %d terminated by signal %s\n",
                g_pid, strsignal(WSTOPSIG(wstatus)));

    else
        warnx("something went wrong");

    g_quit = 1;

    return 0;
}

static void continue_execution(void)
{
    void *addr = (void *) (get_register(MY_REG_RIP) - 1);

    if (is_breakpoint(addr))
    {
        struct my_bp *bp = get_breakpoint(addr);

        toggle_breakpoint(bp);

        set_register(MY_REG_RIP, (size_t) addr);

        // TODO: handle continue signal
        if (ptrace(PTRACE_SINGLESTEP, g_pid, NULL, NULL) == -1)
            goto error;
        
        if (!wait_program())
            return;

        toggle_breakpoint(bp);
    }

    // TODO: handle continue signal
    if (ptrace(PTRACE_CONT, g_pid, NULL, NULL) == -1)
        goto error;

    wait_program();

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
