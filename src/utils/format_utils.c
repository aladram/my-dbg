#include "format_utils.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "binary.h"
#include "exceptions.h"
#include "my_limits.h"
#include "registers.h"
#include "string_utils.h"

void *read_address(char *str)
{
    if (starts_with(str, "%"))
        return (void *) get_register_from_name(str + 1);

    int len = 0;

    void *addr;

    size_t slen = strlen(str);

    if (slen <= 18
        && sscanf(str, "%18p%n", &addr, &len) == 1
        && len == (int) slen)
        return addr;

    if (ex_depth() <= 1)
        warnx("Invalid address specified\n"
              "Valid address example: 0xadead007babe");

    throw(Exception);

    return NULL;
}

size_t read_size(char *str)
{
    if (starts_with(str, "0x") || starts_with(str, "%"))
        return (size_t) read_address(str);

    int len = 0;

    size_t size;

    size_t slen = strlen(str);

    if (slen <= 20
        && (slen < 20 || strcmp(str, "18446744073709551615") <= 0)
        && sscanf(str, "%zu%n", &size, &len) == 1
        && len == (int) slen)
        return size;

    if (ex_depth() <= 1)
        warnx("Invalid number specified\n"
              "Valid number examples: 42, 0x2a");

    throw(Exception);

    return 0;
}

char *get_proc_path(char *name)
{
    static char path[MY_PATH_MAX];

    if (sprintf(path, "/proc/%d/%s", g_pid, name) <= 7 + (int) strlen(name))
        throw(PrintfException);

    return path;
}
