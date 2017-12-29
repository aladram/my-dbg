#include "format_utils.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "exceptions.h"
#include "string_utils.h"

void *read_address(char *str)
{
    int len = 0;

    void *addr;

    size_t slen = strlen(str);

    if (slen <= 18
        && sscanf(str, "%18p%n", &addr, &len) == 1
        && len == (int) slen)
        return addr;

    warnx("Invalid address specified\n"
          "Valid address example: 0xadead007babe");

    throw(Exception);

    return NULL;
}

size_t read_size(char *str)
{
    if (starts_with(str, "0x"))
        return (size_t) read_address(str);

    int len = 0;

    size_t size;

    size_t slen = strlen(str);

    if (slen <= 20
        && (slen < 20 || strcmp(str, "18446744073709551615") <= 0)
        && sscanf(str, "%zu%n", &size, &len) == 1
        && len == (int) slen)
        return size;

    warnx("Invalid number specified\n"
          "Valid number examples: 42, 0x2a");

    throw(Exception);

    return 0;
}
