#include "array_utils.h"
#include <stdarg.h>
#include <stddef.h>

size_t array_length(char **array)
{
    size_t length = 0;

    for (; array[length]; ++length)
        continue;

    return length;
}

int in_array(int value, size_t length, ...)
{
    va_list ap;

    va_start(ap, length);

    int in = 0;

    for (; length && !in; --length)
        if (value == va_arg(ap, int))
            in = 1;

    va_end(ap);

    return in;
}
