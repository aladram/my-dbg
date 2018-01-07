#ifndef COMMANDS_H
# define COMMANDS_H

# include <stddef.h>

enum my_cmd_type
{
    NO_PROGRAM_REQUIRED,
    PROGRAM_REQUIRED
};

struct my_cmd {
    const char *name;

    enum my_cmd_type type;

    const char *description;

    void (*function)(size_t, char **);

    const char *usage;

    const char *alias;
};

#define register_command_with_alias(Name, Type, Function, \
                                    Description, Usage, Alias) \
    static struct my_cmd __cmd_ ## Name \
__attribute__ ((section("cmds"), used)) __attribute__ ((aligned (8))) = \
{ \
    .name = #Name, \
    .type = Type, \
    .description = Description, \
    .function = Function, \
    .usage = Usage, \
    .alias = Alias \
}

#define register_command(Name, Type, Function, Description, Usage) \
        register_command_with_alias(Name, Type, Function, \
                                    Description, Usage, NULL)

void run_command(char *name, char **args);

#endif /* COMMANDS_H */
