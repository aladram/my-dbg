#include <err.h>
#include <stddef.h>
#include <stdio.h>

#include "breakpoints.h"
#include "commands.h"
#include "format_utils.h"
#include "syscalls.h"

static void cmd_break_del(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warnx("No ID specified");

        return;
    }

    size_t nb = read_size(argv[1]);

    if (!nb || nb > g_bp_len)
    {
        warnx("Invalid ID specified");

        return;
    }

    struct my_bp *bp = g_breakpoints + nb - 1;

    if (bp->deleted)
    {
        warnx("Breakpoint %zu already deleted", nb);

        return;
    }

    bp->deleted = 1;

    g_syscalls[bp->word] = -1;

    printf("Breakpoint %zu deleted\n", nb);
}

register_command(break_del,
                 PROGRAM_REQUIRED,
                 cmd_break_del,
                 "Delete a breakpoint",
                 "break_del <ID>");
