#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>

#include "binary.h"
#include "breakpoints.h"
#include "commands.h"
#include "format_utils.h"
#include "memory_utils.h"

static void cmd_break(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warn("No address specified");

        return;
    }

    void *addr = read_address(argv[1]);

    size_t nb = place_breakpoint(addr, 0);

    printf("Breakpoint %zu placed at %p\n", nb, addr);
}

register_command(break,
                 cmd_break,
                 "Set a breakpoint at specified addresses",
                 "break <address>");
