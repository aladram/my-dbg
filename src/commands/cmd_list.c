#include <stddef.h>

#include "commands.h"
#include "my_dwarf.h"

static void cmd_list(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    print_line();
}

register_command(list,
                 PROGRAM_REQUIRED,
                 cmd_list,
                 "Print current source code line",
                 "list");
