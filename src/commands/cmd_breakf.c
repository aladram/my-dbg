#include <err.h>
#include <stddef.h>
#include <stdio.h>

#include "breakpoints.h"
#include "commands.h"
#include "my_elf.h"

static void cmd_breakf(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warnx("No function specified");

        return;
    }

    void *addr = get_address(argv[1]);

    if (!addr)
    {
        warnx("Symbol not found");

        return;
    }

    size_t nb = place_breakpoint(addr, 0);

    printf("Breakpoint %zu placed at %p\n", nb, addr);
}

register_command_with_alias(breakf,
                            PROGRAM_REQUIRED,
                            cmd_breakf,
                            "Set a breakpoint at specified function",
                            "breakf <function>",
                            "bf");
