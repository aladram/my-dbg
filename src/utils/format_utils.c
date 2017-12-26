#include "format_utils.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "string_utils.h"

int read_address(char *str, void **addr)
{
    int len = 0;

    size_t slen = strlen(str);

    if (slen <= 18
        && sscanf(str, "%18p%n", addr, &len) == 1
        && len == (int) slen)
        return 1;

    warnx("Invalid address specified\n"
          "Valid address example: 0xadead007babe");

    *addr = NULL;

    return 0;
}

int read_size(char *str, size_t *size)
{
    if (starts_with(str, "0x"))
        return read_address(str, (void **) size);

    int len = 0;

    size_t slen = strlen(str);

    if (slen <= 20
        && (slen < 20 || strcmp(str, "18446744073709551615") <= 0)
        && sscanf(str, "%zu%n", size, &len) == 1
        && len == (int) slen)
        return 1;

    warnx("Invalid number specified\n"
          "Valid number examples: 42, 0x2a");

    *size = 0;

    return 0;
}
