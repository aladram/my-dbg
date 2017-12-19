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
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        // TODO: communicate between father and child to catch execvp errors

        if (execvp(argv[0], argv) == -1)
            err(1, "%s", argv[0]);
    }

    if (waitpid(g_pid, NULL, 0) == -1)
        err(1, "waitpid failed");
}
