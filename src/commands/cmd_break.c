#include <err.h>
#include <stddef.h>
#include <stdio.h>

#include "breakpoints.h"
#include "commands.h"
#include "format_utils.h"

static void cmd_break(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warnx("No address specified");

        return;
    }

    void *addr = read_address(argv[1]);

    size_t nb = place_breakpoint(addr, 0);

    printf("Breakpoint %zu placed at %p\n", nb, addr);
}

register_command_with_alias(break,
                            PROGRAM_REQUIRED,
                            cmd_break,
                            "Set a breakpoint at specified addresses",
                            "break <address>",
                            "b");
