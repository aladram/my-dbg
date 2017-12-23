#include <err.h>
#include <signal.h>
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

static int signum;

static int wait_program(int step)
{
    int wstatus;

    if (waitpid(g_pid, &wstatus, 0) == -1)
    {
        warn("waitpid failed");

        return 0;
    }

    if (WIFSTOPPED(wstatus))
    {
        signum = WSTOPSIG(wstatus);

        if (step && signum == SIGTRAP)
        {
            signum = 0;

            return 1;
        }

        void *addr = (void *) (get_register(MY_REG_RIP) - 1);

        if (is_breakpoint(addr))
        {
            struct my_bp *bp = get_breakpoint(addr);

            printf("Breakpoint %zu at %p\n", bp->id, addr);

            if (signum == SIGTRAP)
                signum = 0;
        }

        if (signum)
            printf("Process %d interrupted by signal %s\n",
                   g_pid, strsignal(signum));

        return 0;
    }

    if (WIFEXITED(wstatus))
        printf("Process %d exited with code %hhd\n",
               g_pid, WEXITSTATUS(wstatus));

    else if (WIFSIGNALED(wstatus))
        printf("Process %d terminated by signal %s\n",
               g_pid, strsignal(WTERMSIG(wstatus)));

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
        if (ptrace(PTRACE_SINGLESTEP, g_pid, NULL, signum) == -1)
            goto error;

        signum = 0;
        
        if (!wait_program(1))
            return;

        toggle_breakpoint(bp);
    }

    // TODO: handle continue signal
    if (ptrace(PTRACE_CONT, g_pid, NULL, signum) == -1)
        goto error;

    signum = 0;

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
