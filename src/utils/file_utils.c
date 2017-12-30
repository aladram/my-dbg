#include "file_utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "exceptions.h"

size_t file_size(char *path)
{
    struct stat sb;

    if (stat(path, &sb) == -1)
        throw(IOException);

    return (size_t) sb.st_size;
}
