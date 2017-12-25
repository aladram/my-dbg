#include <stddef.h>

#include "binary.h"
#include "commands.h"

static void cmd_step_instr(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    single_step();
}

register_command(step_instr,
                 cmd_step_instr,
                 "Step one instruction exactly",
                 "step_instr");
