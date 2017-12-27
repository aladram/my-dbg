#include "binary.h"

#include <err.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "breakpoints.h"
#include "my-dbg.h"
#include "registers.h"

pid_t g_pid;

int g_signum;

void setup_binary(char **argv)
{
    g_pid = fork();

    if (g_pid == -1)
        err(1, "fork failed");

    if (!g_pid)
    {
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
            err(1, "ptrace failed");

        if (execv(argv[0], argv) == -1)
            if (execvp(argv[0], argv) == -1)
                err(1, "%s", argv[0]);
    }

    int wstatus;

    if (waitpid(g_pid, &wstatus, 0) == -1)
        err(1, "waitpid failed");

    if (!WIFSTOPPED(wstatus) || WSTOPSIG(wstatus) != SIGTRAP)
        errx(1, "An error occured while trying to debug program, exiting");
}

int wait_program(int step)
{
    int wstatus;

    if (waitpid(g_pid, &wstatus, 0) == -1)
    {
        warn("waitpid failed");

        return 0;
    }

    if (WIFSTOPPED(wstatus))
    {
        g_signum = WSTOPSIG(wstatus);

        if (step && g_signum == SIGTRAP)
        {
            g_signum = 0;

            return 1;
        }

        void *addr = (void *) (get_register(MY_REG_RIP) - 1);

        if (is_breakpoint(addr))
        {
            struct my_bp *bp = get_breakpoint(addr);

            printf("Breakpoint %zu at %p\n", bp->id, addr);

            set_register(MY_REG_RIP, (size_t) addr);

            if (g_signum == SIGTRAP)
                g_signum = 0;
        }

        if (g_signum)
            printf("Process %d interrupted by signal %s\n",
                   g_pid, strsignal(g_signum));

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

/*
* Returns 0 if, after single step, program is interrupted
* (breakpoint, signal) or terminated, 0 else.
*/
int single_step(void)
{
    void *addr = (void *) get_register(MY_REG_RIP);

    struct my_bp *bp = NULL;

    if (is_breakpoint(addr))
    {
        bp = get_breakpoint(addr);

        toggle_breakpoint(bp);
    }

    if (ptrace(PTRACE_SINGLESTEP, g_pid, NULL, g_signum) == -1)
        goto error;

    g_signum = 0;

    int ret = wait_program(1);

    if (bp)
        toggle_breakpoint(bp);

    return ret;

error:
    warn("ptrace failed");

    return 0;
}
