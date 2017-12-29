#ifndef COMMANDS_H
# define COMMANDS_H

# include <stddef.h>

struct my_cmd {
    const char *name;

    const char *description;

    void (*function)(size_t, char **);

    const char *usage;

    const char *alias;
};

#define register_command_with_alias(Name, Function, Description, Usage, Alias) \
    static struct my_cmd __cmd_ ## Name \
__attribute__ ((section("cmds"), used)) = \
{ \
    .name = #Name, \
    .description = Description, \
    .function = Function, \
    .usage = Usage, \
    .alias = Alias \
}

#define register_command(Name, Function, Description, Usage) \
        register_command_with_alias(Name, Function, Description, Usage, NULL)

void run_command(char *name, char **args);

#endif /* COMMANDS_H */
