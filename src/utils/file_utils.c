#include "file_utils.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "exceptions.h"

size_t file_size(FILE *f)
{
    int fd = fileno(f);

    if (fd == -1)
        throw(IOException);

    struct stat sb;

    if (fstat(fd, &sb) == -1)
        throw(IOException);

    return (size_t) sb.st_size;
}
