#include <err.h>
#include <stddef.h>
#include <stdio.h>

#include "syscalls.h"
#include "commands.h"
#include "format_utils.h"

static void cmd_breaks(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warnx("No syscall number specified");

        return;
    }

    size_t syscall = read_size(argv[1]);

    add_syscall_break((int) syscall);

    printf("Breakpoint placed on syscall %zu\n", syscall);
}

register_command_with_alias(breaks,
                            PROGRAM_REQUIRED,
                            cmd_breaks,
                            "Set a breakpoint at specified syscall",
                            "breaks <syscall number>",
                            "bs");
