#ifndef FILE_UTILS_H
# define FILE_UTILS_H

# include <stddef.h>

size_t file_size(char *path);

char *real_path(char *path);

void print_file_line(char *dir, char *file, unsigned line);

#endif /* FILE_UTILS_H */
