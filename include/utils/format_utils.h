#ifndef FORMAT_UTILS_H
# define FORMAT_UTILS_H

# include <stddef.h>

int read_address(char *str, void **addr);

int read_size(char *str, size_t *size);

#endif /* FORMAT_UTILS_H */
