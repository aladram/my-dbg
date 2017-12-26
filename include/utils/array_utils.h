#ifndef ARRAY_UTILS_H
# define ARRAY_UTILS_H

# include <stddef.h>

size_t array_length(char **array);

int in_array(int value, size_t length, ...);

#endif /* ARRAY_UTILS_H */
