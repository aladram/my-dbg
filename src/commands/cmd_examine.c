#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "array_utils.h"
#include "commands.h"
#include "format_utils.h"
#include "memory.h"

void examine_memory(char format, void *addr, size_t size)
{
    char *mem = read_memory(addr, size);

    if (!mem)
        return;

    switch (format)
    {
    case 'x':
        for (size_t i = 0; i < size; ++i)
            printf("%s%s%02hhx",
                   (i && !(i % 16)) ? "\n" : "",
                   (i && !(i % 2) && i % 16) ? " " : "",
                   mem[i]);

        printf("\n");

        break;

    default:
        assert("Not implemented");
    }

}

static void cmd_examine(size_t argc, char **argv)
{
    if (argc < 4)
    {
        warnx("Missing arguments");

        return;
    }

    if (strlen(argv[1]) != 1 || !in_array(argv[1][0], 4, 'x', 'd', 'i', 's'))
    {
        warnx("Invalid format argument\nValid formats: x, d, i and s");

        return;
    }

    size_t size;

    if (!read_size(argv[2], &size))
        return;

    void *addr;

    if (!read_address(argv[3], &addr))
        return;

    if (size)
        examine_memory(argv[1][0], addr, size);

    else
        warnx("Non-null size required");
}

register_command(examine,
                 cmd_examine,
                 "Examine memory at given address",
                 "examine <format> <size> <start_addr>");
