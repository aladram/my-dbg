#include <stddef.h>
#include <stdlib.h>

#include "commands.h"

static void cmd_quit(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    exit(0);
}

register_command(quit, cmd_quit, "Exit my-dbg");
