#include <stddef.h>
#include <stdio.h>

#include "binary.h"
#include "breakpoints.h"
#include "commands.h"
#include "libunwind_wrapper.h"

static void cmd_finish(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    void *addr = get_finish_address();

    place_breakpoint(addr, MY_BP_TEMP | MY_BP_INTERNAL);

    continue_execution();

    printf("Continued to %p\n", (void *) addr);
}

register_command(finish,
                 PROGRAM_REQUIRED,
                 cmd_finish,
                 "Ends current function",
                 "finish");
