#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "binary.h"
#include "breakpoints.h"
#include "capstone_wrapper.h"
#include "commands.h"
#include "registers.h"
#include "string_utils.h"

static void cmd_next_instr(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    size_t rip = get_register(MY_REG_RIP);

    if (!rip)
        return;

    struct my_instr *instr = get_current_instruction();

    if (!instr)
        return;

    if (starts_with(instr->str, "call"))
    {
        place_breakpoint((void *) (rip + instr->size), 1);

        continue_execution();
    }

    else
        single_step();

    rip = get_register(MY_REG_RIP);

    if (!rip)
        return;

    printf("Stepped to %p\n", (void *) rip);
}

register_command(next_instr,
                 cmd_next_instr,
                 "Step one instruction, but proceed through calls",
                 "next_instr");
