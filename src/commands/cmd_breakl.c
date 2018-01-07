#include <err.h>
#include <stddef.h>
#include <stdio.h>

#include "breakpoints.h"
#include "commands.h"
#include "format_utils.h"
#include "my_dwarf.h"

static void cmd_breakl(size_t argc, char **argv)
{
    if (argc <= 2)
    {
        warn("Missing arguments");

        return;
    }

    size_t line = read_size(argv[1]);

    void *addr = get_line_address(argv[2], line);

    place_breakpoint(addr, MY_BP_TEMP | MY_BP_INTERNAL);

    printf("Breakpoint placed at %p\n", (void *) addr);
}

register_command_with_alias(breakl,
                            PROGRAM_REQUIRED,
                            cmd_breakl,
                            "Ends current function",
                            "breakl",
                            "bl");
