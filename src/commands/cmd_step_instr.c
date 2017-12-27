#include <stddef.h>
#include <stdio.h>

#include "binary.h"
#include "commands.h"
#include "registers.h"

static void cmd_step_instr(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    single_step();

    size_t rip = get_register(MY_REG_RIP);

    if (rip)
        printf("Stepped to %p\n", (void *) rip);
}

register_command(step_instr,
                 cmd_step_instr,
                 "Step one instruction exactly",
                 "step_instr");
