#include <err.h>
#include <stddef.h>
#include <sys/ptrace.h>

#include "binary.h"
#include "commands.h"
#include "my-dbg.h"

static void cmd_quit(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    if (ptrace(PTRACE_KILL, g_pid, NULL, NULL) == -1)
        warn("ptrace failed");

    g_quit = 1;
}

register_command(quit,
                 cmd_quit,
                 "Exit my-dbg",
                 "exit");
