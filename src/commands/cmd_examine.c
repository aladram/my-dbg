#include <capstone/capstone.h>
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

static void examine_instructions(void *addr, char *mem, size_t size)
{
    csh handle;

    cs_insn *insn;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return;

    cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);

    size_t count = cs_disasm(handle, (uint8_t *) mem, size, 0, 0, &insn);

    if (count)
    {
        for (size_t i = 0; i < count; i++)
            printf("%-18p%-16s%s\n", (void *) ((size_t) addr + insn[i].address),
                   insn[i].mnemonic, insn[i].op_str);

        cs_free(insn, count);
    }

    else
        warnx("Dissasemble failed");

    cs_close(&handle);
}

static void examine_memory(char format, void *addr, size_t size)
{
    char *mem = read_memory(addr, size);

    if (!mem)
        return;

    if (format == 'x')
        examine_hexa(mem, size);

    else if (format == 'd')
        examine_decimal(mem, size);

    else if (format == 's')
        examine_string(mem, size);

    else if (format == 'i')
        examine_instructions(addr, mem, size);

    free(mem);
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

    size_t size = read_size(argv[2]);

    void *addr = read_address(argv[3]);

    if (size)
        examine_memory(argv[1][0], addr, size);

    else
        warnx("Nothing to examine (null size)");
}

register_command(examine,
                 PROGRAM_REQUIRED,
                 cmd_examine,
                 "Examine memory at given address",
                 "examine <format> <size> <start_addr>");
