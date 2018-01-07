#include "file_utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "exceptions.h"
#include "my_limits.h"

size_t file_size(char *path)
{
    struct stat sb;

    if (stat(path, &sb) == -1)
        throw(IOException);

    return (size_t) sb.st_size;
}

char *real_path(char *path)
{
    static char buf[MY_PATH_MAX];

    if (readlink(path, buf, MY_PATH_MAX) <= 0)
        throw(IOException);

    buf[strrchr(buf, '/') - buf] = 0;

    return buf;
}

void print_file_line(char *dir, char *file, unsigned line)
{
    char path[MY_PATH_MAX];

    int ret = sprintf(path, "%s/%s", dir ? dir : "", file);

    if (ret == -1 || ret != (int) (strlen(dir) + strlen(file) + 1))
        throw(PrintfException);

    FILE *f = fopen(path, "r");

    if (!f)
        throw(IOException);

    for (unsigned l = 1; l < line; ++l)
        for (int c = fgetc(f); c != '\n'; c = fgetc(f))
            if (c == EOF)
                throw(IOException);

    char *lineptr = NULL;

    size_t n = 0;

    if (getline(&lineptr, &n, f) == -1 || !lineptr)
        throw(IOException);

    if (fputs(lineptr, stdout) == EOF)
        throw(IOException);
}
