#ifndef MEM_MAPPINGS_H
# define MEM_MAPPINGS_H

# include <stddef.h>

struct my_mem_mapping
{
    void *start;

    void *end;

    size_t offset;

    char *objfile;
};

struct my_mem_mapping **get_mem_mappings(void);

void free_mem_mappings(struct my_mem_mapping **mappings);

struct my_mem_mapping *find_mem_mappings(struct my_mem_mapping **mappings,
                                         void *addr);

#endif /* MEM_MAPPINGS_H */
