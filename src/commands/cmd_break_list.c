#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "breakpoints.h"
#include "commands.h"
#include "exceptions.h"
#include "syscalls.h"

#define PRINT_STR(Bool, Yes, No) ((Bool) ? Yes : No)
#define PRINT_BOOL(Bool) PRINT_STR(Bool, "yes", "no")

static void cmd_break_list(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    printf("%-6s %-10s %-9s %-7s %s\n",
           "ID",
           "Type",
           "Temporary",
           "Enabled",
           "What");

    for (size_t i = 0; i < g_bp_len; ++i)
    {
        struct my_bp *bp = g_breakpoints + i;

        if (bp->flags & MY_BP_INTERNAL)
            continue;

        printf("%-6zu %-10s %-9s %-7s ",
               bp->id,
               PRINT_STR(bp->flags & MY_BP_SYSCALL, "syscall", "breakpoint"),
               PRINT_BOOL(bp->flags & MY_BP_TEMP),
               PRINT_BOOL(bp->enabled));

        if (bp->flags & MY_BP_SYSCALL)
        {
            int syscall = (int) (uintptr_t) bp->addr;

            try
            {
                printf("Syscall: %s (%d)", syscall_name(syscall), syscall);
            }
            catch (SyscallException)
            {
                printf("Syscall: %d", syscall);
            }
            etry;
        }

        else
            printf("Address: %p", bp->addr);

        printf("\n");
    }
}

register_command(break_list,
                 PROGRAM_REQUIRED,
                 cmd_break_list,
                 "List breakpoints",
                 "break_list");
