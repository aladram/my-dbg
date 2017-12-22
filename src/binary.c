#include "binary.h"

#include <err.h>
#include <stddef.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t g_pid;

void setup_binary(char **argv)
{
    g_pid = fork();

    if (g_pid == -1)
        err(1, "fork failed");

    if (!g_pid)
    {
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
            err(1, "ptrace failed");

        if (execvp(argv[0], argv) == -1)
            err(1, "%s", argv[0]);
    }

    int wstatus;

    if (waitpid(g_pid, &wstatus, 0) == -1)
        err(1, "waitpid failed");

    if (!WIFSTOPPED(wstatus) || WSTOPSIG(wstatus) != SIGTRAP)
        errx(1, "An error occured while trying to debug program, exiting");
}
