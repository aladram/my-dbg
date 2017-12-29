#include "commands.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "array_utils.h"
#include "errors.h"
#include "exceptions.h"
#include "string_utils.h"

extern struct my_cmd __start_cmds[];

extern struct my_cmd __stop_cmds[];

static struct my_cmd *get_command(char *name)
{
    struct my_cmd *cmd_matched = NULL;

    size_t start = (size_t) __start_cmds;

    size_t end = (size_t) __stop_cmds;

    for (size_t i = 0; i < (end - start) / sizeof(struct my_cmd); ++i)
    {
        struct my_cmd *cmd = __start_cmds + i;

        if (!strcmp(name, cmd->name))
            return cmd;

        if (starts_with(cmd->name, name))
        {
            if (cmd_matched)
            {
                warnx("%s: ambigious command name", name);

                return NULL;
            }

            cmd_matched = cmd;
        }
    }

    if (cmd_matched)
        return cmd_matched;

    warnx("%s: command not found", name);

    return NULL;
}

void run_command(char *name, char **args)
{
    struct my_cmd *cmd = get_command(name);

    if (!cmd)
        return;

    try
    {
        (cmd->function)(array_length(args), args);
    }
    catch (PtraceException)
    {
        ptrace_error();
    }
    catch (WaitException)
    {
        wait_error();
    }
    catch (ScanfException)
    {
        sscanf_error();
    }
    catch (PrintfException)
    {
        printf_error();
    }
    catch (IOException)
    {
        io_error();
    }
    catch (AllocException)
    {
        alloc_error();
    }
    catch (Exception)
    {
        warnx("An error occured while trying to execute command %s", cmd->name);
    }
    etry;
}

static void print_help(struct my_cmd *cmd)
{
    if (cmd)
        printf("%s: %s\n", cmd->usage, cmd->description);
}

static void cmd_help(size_t argc, char **argv)
{
    if (argc > 1)
    {
        for (size_t i = 1; i < argc; ++i)
        {
            struct my_cmd *cmd = get_command(argv[i]);

            print_help(cmd);
        }

        return;
    }

    size_t start = (size_t) __start_cmds;

    size_t end = (size_t) __stop_cmds;

    for (size_t i = 0; i < (end - start) / sizeof(struct my_cmd); ++i)
    {
        struct my_cmd *cmd = __start_cmds + i;

        print_help(cmd);
    }
}

register_command(help,
                 cmd_help,
                 "Display this help message, or specified commands help",
                 "help [command ...]");
