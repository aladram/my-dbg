#include "mem_mappings.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary.h"
#include "exceptions.h"
#include "format_utils.h"
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
        throw(ScanfException);

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

    char *str = NULL;

    enum my_exception my_ex;

    try
    {
        while (1)
        {
            size_t len = 0;

            errno = 0;

            if (getline(&str, &len, f) == -1)
            {
                if (errno)
                    throw(IOException);

                free(str);

                break;
            }

            length = add_mem_mapping(&mappings, str, length);

            free(str);
        }

        add_mem_mapping(&mappings, NULL, length);

        delete_env();

        return mappings;
    }
    catch (ScanfException, IOException)
    {
        free(str);

        free_mem_mappings(mappings);

        my_ex = ex;
    }
    etry;

    throw(my_ex);

    return NULL;
}

struct my_mem_mapping **get_mem_mappings(void)
{
    char *path = get_proc_path("maps");

    FILE *f = fopen(path, "re");

    if (!f)
    {
        warn("%s", path);

        throw(IOException);
    }

    struct my_mem_mapping **mappings = get_mem_mappings_file(f);

    if (fclose(f) == EOF)
    {
        warn("%s", path);

        throw(IOException);
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
