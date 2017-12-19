#include "array_utils.h"
#include <stddef.h>

size_t array_length(char **array)
{
    size_t length = 0;

    for (; array[length]; ++length)
        continue;

    return length;
}
