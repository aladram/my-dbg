#include "memory.h"

#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "binary.h"
#include "format_utils.h"
#include "memory_utils.h"

char *read_memory(void *addr, size_t size)
{
    char *mem_path = get_proc_path("mem");

    int fd = open(mem_path, O_RDONLY);

    if (fd == -1)
    {
        warn("%s", mem_path);

        return NULL;
    }

    /*
    * Manual malloc error management here to prevent program stopping
    * if read_memory fails because user asked for too large memory
    * page using examine command.
    */
    char *buf = calloc((size / sizeof(size_t)
                        + ((size % sizeof(size_t)) ? 1 : 0))
                       * sizeof(size_t), 1);

    if (!buf)
    {
        warn("malloc failed");

        close(fd);

        return NULL;
    }

    ssize_t ret = pread(fd, buf, size, (off_t) addr);

    if (ret == -1)
        warn("pread failed");

    else if ((size_t) ret == size)
    {
        close(fd);

        return buf;
    }

    else
        warnx("Cannot read the whole requested memory");

    free(buf);

    close(fd);

    return NULL;
}

static size_t add_char(char **buf, size_t length, char c)
{
    *buf = my_realloc(*buf, ++length);

    (*buf)[length - 1] = c;

    return length;
}

char *read_mem_string(void *addr)
{
    char *mem_path = get_proc_path("mem");

    int fd = open(mem_path, O_RDONLY);

    if (fd == -1)
    {
        warn("%s", mem_path);

        return NULL;
    }

    char *buf = NULL;

    size_t length = 0;

    char c = 0;

    ssize_t ret = pread(fd, &c, 1, (off_t) addr + length);

    for (; c && ret == 1; ret = pread(fd, &c, 1, (off_t) addr + length))
        length = add_char(&buf, length, c);

    if (ret == -1)
    {
        free(buf);

        buf = NULL;
    }

    add_char(&buf, length, 0);

    close(fd);

    return buf;
}
