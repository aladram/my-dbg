#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary.h"
#include "commands.h"
#include "memory_utils.h"
#include "my_limits.h"

struct my_mem_mapping
{
    void *start;

    void *end;

    size_t offset;

    char *objfile;
};

static size_t add_mem_mapping(struct my_mem_mapping **mappings,
                              char *line,
                              size_t length)
{
    *mappings = my_realloc(*mappings, ++length * sizeof(**mappings));

    struct my_mem_mapping *mapping = (*mappings) + length - 1;

    /*char *saveptr = NULL;

    str = strtok_r(str, " \t", &saveptr);

    for (; str; str = strtok_r(NULL, " \t", &saveptr))*/

    char c = 0;

    unsigned long long u = 0;

    mapping->objfile = my_calloc(MY_PATH_MAX, 1);

    int ret = sscanf(line,
                     "%zx-%zx %c%c%c%c %8zx %2llu:%2llu %llu %s",
                     (size_t *) &mapping->start,
                     (size_t *) &mapping->end,
                     &c, &c, &c, &c,
                     &mapping->offset,
                     &u, &u, &u,
                     mapping->objfile);

    if (ret != 10 && ret != 11)
        return 0;

    return length;
}

static size_t get_mem_mappings_file(FILE *f, struct my_mem_mapping **mappings)
{
    size_t length = 0;

    while (1)
    {
        char *str = NULL;

        size_t len = 0;

        errno = 0;

        if (getline(&str, &len, f) == -1)
        {
            if (errno)
            {
                warn("getline failed");

                goto error;
            }

            break;
        }

        length = add_mem_mapping(mappings, str, length);

        if (length == 0)
        {
            warn("sscanf failed");

            goto error;
        }
    }

    return length;

error:
    free(*mappings);

    *mappings = NULL;

    return 0;
}

/*
* Address pointed by mappings needs to be NULL.
* On error, it will stay NULL.
*/
static size_t get_mem_mappings(struct my_mem_mapping **mappings)
{
    char path[32];

    if (sprintf(path, "/proc/%d/maps", g_pid) <= 11)
    {
        warn("sprintf failed");

        return 0;
    }

    FILE *f = fopen(path, "re");

    if (!f)
    {
        warn("%s", path);

        return 0;
    }

    size_t length = get_mem_mappings_file(f, mappings);

    if (fclose(f) == EOF)
    {
        warn("%s", path);

        return 0;
    }

    return length;
}

static void cmd_info_memory(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    struct my_mem_mapping *mappings = NULL;

    size_t length = get_mem_mappings(&mappings);

    printf("%18s %18s %10s %10s %s\n",
           "Start Addr",
           "End Addr",
           "Size",
           "Offset",
           "Objfile");

    for (size_t i = 0; i < length; ++i)
    {
        struct my_mem_mapping *mapping = mappings + i;

        printf("%18p %18p %#10zx %#10zx %s\n",
               mapping->start,
               mapping->end,
               (size_t) mapping->end - (size_t) mapping->start,
               mapping->offset,
               mapping->objfile);
    }
}

register_command(info_memory,
                 cmd_info_memory,
                 "Display mapped memory regions",
                 "info_memory");
