#include <err.h>
#include <stddef.h>

#include "capstone_wrapper.h"
#include "commands.h"
#include "format_utils.h"

static void cmd_disassemble(size_t argc, char **argv)
{
    if (argc <= 2)
    {
        warnx("Missing arguments");

        return;
    }

    void *addr = read_address(argv[1]);

    size_t count = read_size(argv[2]);

    if (count)
        print_instructions(addr, count);

    else
        warnx("Nothing to disassemble (null size)");
}

register_command(disassemble,
                 PROGRAM_REQUIRED,
                 cmd_disassemble,
                 "Disassemble N instructions at specified address",
                 "disassemble <address> <N>");
