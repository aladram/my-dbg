#ifndef MEMORY_H
# define MEMORY_H

# include <stddef.h>

char *read_memory(void *addr, size_t size);

char *read_mem_string(void *addr);

#endif /* MEMORY_H */
