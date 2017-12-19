#include "string_utils.h"
#include <string.h>

int starts_with(const char *haystack, const char *needle)
{
    if (strlen(haystack) < strlen(needle))
        return 0;

    return !strncmp(haystack, needle, strlen(needle));
}
