#ifndef TEMP_MEMORY_UTILS_H
# define TEMP_MEMORY_UTILS_H

# include <stddef.h>

void tmp_free_all(void);

void *tmp_malloc(size_t size);

void *tmp_realloc(void *old_ptr, size_t old_size, size_t new_size);

void *tmp_calloc(size_t nmemb, size_t size);

char *tmp_strdup(const char *s);

#endif /* TEMP_MEMORY_UTILS_H */
