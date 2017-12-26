#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "array_utils.h"
#include "commands.h"
#include "format_utils.h"
#include "memory.h"

static void examine_hexa(char *mem, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        printf("%s%s%02hhx",
               (i && !(i % 16)) ? "\n" : "",
               (i && !(i % 2) && i % 16) ? " " : "",
               mem[i]);

    printf("\n");
}

static void examine_decimal(char *mem, size_t size)
{
    size = size / sizeof(int) + ((size % sizeof(int)) ? 1 : 0);

    int *mem_int = (int *) mem;

    for (size_t i = 0; i < size; ++i)
        printf("%s%11d",
               (i && !(i % 2)) ? "\n" : "",
               mem_int[i]);

    printf("\n");
}

static int is_special_char(char c)
{
    return in_array(c, 12, '\a', '\b', '\f', '\n', '\r', '\f', '\t', '\v',
                    '\\', '\'', '\"', '\?');
}

static char get_special_char(char c)
{
    if (c == '\a')
        return 'a';

    else if (c == '\b')
        return 'b';

    else if (c == '\f')
        return 'f';

    else if (c == '\n')
        return 'n';

    else if (c == '\r')
        return 'r';

    else if (c == '\f')
        return 'f';

    else if (c == '\t')
        return 't';

    else if (c == '\v')
        return 'v';

    return c;
}

static void examine_string(char *mem, size_t size)
{
    printf("\"");

    for (size_t i = 0; i < size; ++i)
    {
        if (is_special_char(mem[i]))
            printf("\\%c", get_special_char(mem[i]));

        else if (!isprint(mem[i]))
            printf("\\x%02hhx", mem[i]);

        else
            printf("%c", mem[i]);
    }

    printf("\"\n");
}

static void examine_memory(char format, void *addr, size_t size)
{
    char *mem = read_memory(addr, size);

    if (!mem)
        return;

    switch (format)
    {
    case 'x':
        examine_hexa(mem, size);

        break;

    case 'd':
        examine_decimal(mem, size);

        break;

    case 's':
        examine_string(mem, size);

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
