#include <stddef.h>
#include <stdio.h>

#include "binary.h"
#include "commands.h"

static void cmd_continue(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    puts("Continuing.");

    continue_execution();
}

register_command(continue,
                 PROGRAM_REQUIRED,
                 cmd_continue,
                 "Continue program execution",
                 "continue");
