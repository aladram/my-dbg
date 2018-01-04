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

    struct my_instr *instr = get_current_instruction();

    if (!instr)
        return;

    int over = starts_with(instr->str, "call");

    if (over)
    {
        place_breakpoint((void *) (rip + instr->size),
                         MY_BP_TEMP | MY_BP_INTERNAL);

        continue_execution();
    }

    else
        single_step();

    rip = get_register(MY_REG_RIP);

    printf("Stepped%s to %p\n", over ? " call" : "", (void *) rip);
}

register_command(next_instr,
                 PROGRAM_REQUIRED,
                 cmd_next_instr,
                 "Step one instruction, but proceed through calls",
                 "next_instr");
