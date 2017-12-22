#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>

#include "binary.h"
#include "breakpoints.h"
#include "commands.h"
#include "memory_utils.h"

static void cmd_break(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warn("No address specified");

        return;
    }

    void *addr;

    int len = 0;

    if (strlen(argv[1]) > 18
        || sscanf(argv[1], "%18p%n", &addr, &len) != 1
        || len != (int) strlen(argv[1]))
    {
        warnx("Invalid address specified\nValid address example: 0xcafebabe");

        return;
    }

    size_t nb = place_breakpoint(addr);

    if (nb)
        printf("Breakpoint %zu at %p\n", nb, addr);
}

register_command(break,
                 cmd_break,
                 "Set a breakpoint at specified addresses",
                 "break <address>");
