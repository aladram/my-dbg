#ifndef FORMAT_UTILS_H
# define FORMAT_UTILS_H

# include <stddef.h>

void *read_address(char *str);

size_t read_size(char *str);

char *get_proc_path(char *name);

#endif /* FORMAT_UTILS_H */
