#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "mem_mappings.h"

static void cmd_info_memory(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    struct my_mem_mapping **mappings = get_mem_mappings();

    if (!mappings)
    {
        warn("No memory mapping found");

        return;
    }

    printf("%18s %18s %10s %10s %s\n",
           "Start Addr",
           "End Addr",
           "Size",
           "Offset",
           "Objfile");

    for (size_t i = 0; mappings[i]; ++i)
    {
        struct my_mem_mapping *mapping = mappings[i];

        printf("%18p %18p %#10zx %#10zx %s\n",
               mapping->start,
               mapping->end,
               (size_t) mapping->end - (size_t) mapping->start,
               mapping->offset,
               mapping->objfile);
    }

    free_mem_mappings(mappings);
}

register_command(info_memory,
                 PROGRAM_REQUIRED,
                 cmd_info_memory,
                 "Display mapped memory regions",
                 "info_memory");
