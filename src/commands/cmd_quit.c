#include <stddef.h>

#include "commands.h"
#include "my-dbg.h"

static void cmd_quit(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    g_quit = 1;
}

register_command(quit, cmd_quit, "Exit my-dbg");
