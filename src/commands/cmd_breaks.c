#include <err.h>
#include <stddef.h>
#include <stdio.h>

#include "exceptions.h"
#include "syscalls.h"
#include "commands.h"
#include "format_utils.h"

static void cmd_breaks(size_t argc, char **argv)
{
    if (argc <= 1)
    {
        warnx("No syscall specified");

        return;
    }

    int syscall = 0;

    enum my_exception my_ex = None;

    try
    {
        syscall = (int) read_size(argv[1]);
    }
    catch (Exception)
    {
        try
        {
            syscall = syscall_num(argv[1]);
        }
        catch (SyscallException)
        {
            warnx("Unknown syscall name '%s'", argv[1]);

            my_ex = ex;
        }
        etry;
    }
    etry;

    if (my_ex != None)
        throw(my_ex);

    add_syscall_break(syscall);

    try
    {
        printf("Breakpoint placed on syscall %s (%d)\n",
               syscall_name(syscall),
               syscall);
    }
    catch (SyscallException)
    {
        printf("Breakpoint placed on syscall %d\n",
               syscall);
    }
    etry;
}

register_command_with_alias(breaks,
                            PROGRAM_REQUIRED,
                            cmd_breaks,
                            "Set a breakpoint on specified syscall",
                            "breaks <syscall | syscall number>",
                            "bs");
