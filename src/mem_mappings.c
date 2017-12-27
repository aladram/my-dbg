#include "mem_mappings.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary.h"
#include "memory_utils.h"
#include "my_limits.h"

static size_t add_mem_mapping(struct my_mem_mapping ***mappings,
                              char *line,
                              size_t length)
{
    *mappings = my_realloc(*mappings, ++length * sizeof(**mappings));

    if (!line)
    {
        (*mappings)[length - 1] = NULL;

        return length;
    }

    (*mappings)[length - 1] = my_malloc(sizeof(***mappings));

    struct my_mem_mapping *mapping = (*mappings)[length - 1];

    char c = 0;

    unsigned long long u = 0;

    mapping->objfile = my_calloc(MY_PATH_MAX, 1);

    int ret = sscanf(line,
                     "%zx-%zx %c%c%c%c %8zx %2llx:%2llx %llu %s",
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

void free_mem_mappings(struct my_mem_mapping **mappings)
{
    if (!mappings)
        return;

    for (size_t i = 0; mappings[i]; ++i)
    {
        free(mappings[i]->objfile);

        free(mappings[i]);
    }

    free(mappings);
}

static struct my_mem_mapping **get_mem_mappings_file(FILE *f)
{
    struct my_mem_mapping **mappings = NULL;

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

            free(str);

            break;
        }

        length = add_mem_mapping(&mappings, str, length);

        free(str);

        if (length == 0)
        {
            warn("sscanf failed");

            goto error;
        }
    }

    add_mem_mapping(&mappings, NULL, length);

    return mappings;

error:
    free_mem_mappings(mappings);

    return NULL;
}

struct my_mem_mapping **get_mem_mappings(void)
{
    char path[32];

    if (sprintf(path, "/proc/%d/maps", g_pid) <= 11)
    {
        warn("sprintf failed");

        return NULL;
    }

    FILE *f = fopen(path, "re");

    if (!f)
    {
        warn("%s", path);

        return NULL;
    }

    struct my_mem_mapping **mappings = get_mem_mappings_file(f);

    if (fclose(f) == EOF)
    {
        warn("%s", path);

        return NULL;
    }

    return mappings;
}

struct my_mem_mapping *find_mem_mappings(struct my_mem_mapping **mappings,
                                         void *addr)
{
    for (size_t i = 0; mappings[i]; ++i)
    {
        struct my_mem_mapping *mapping = mappings[i];

        if (mapping->start <= addr && addr < mapping->end)
            return mapping;
    }

    return NULL;
}
