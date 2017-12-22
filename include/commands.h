#ifndef COMMANDS_H
# define COMMANDS_H

# include <stddef.h>

struct my_cmd {
    const char *name;

    const char *description;

    void (*function)(size_t, char **);

    const char *usage;
};

#define register_command(Name, Function, Description, Usage) \
    static struct my_cmd __cmd_ ## Name \
__attribute__ ((section("cmds"), used)) = \
{ \
    .name = #Name, \
    .description = Description, \
    .function = Function, \
    .usage = Usage \
}

void run_command(char *name, char **args);

#endif /* COMMANDS_H */
