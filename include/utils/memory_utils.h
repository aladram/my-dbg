#ifndef MEMORY_UTILS_H
# define MEMORY_UTILS_H

# include <stddef.h>

void *my_malloc(size_t size);

void *my_calloc(size_t nmemb, size_t size);

void *my_realloc(void *ptr, size_t size);

#endif /* MEMORY_UTILS_H */
