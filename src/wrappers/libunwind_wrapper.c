#include "libunwind_wrapper.h"

#include <dlfcn.h>
#include <err.h>
#include <libunwind.h>
#include <libunwind-ptrace.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "binary.h"
#include "mem_mappings.h"

#define MY_MAX_BT_DEPTH 64

#define MY_UNW_EINVAL "unw_init_remote() was called in a version" \
                      " of libunwind which supports local unwinding only"

#define MY_UNW_EUNSPEC "An unspecified error occurred."

#define MY_UNW_EBADREG "A register needed by unw_init_remote()" \
                       " wasn't accessible."

#define MY_UNW_ENOINFO "Libunwind was unable to locate the unwind-info" \
                       " needed to complete the operation."

#define MY_UNW_EBADVERSION "The unwind-info needed to complete" \
                           " the operation has a version" \
                           " or a format that is not" \
                           " understood by libunwind."

#define MY_UNW_EINVALIDIP "The  instruction-pointer  (``program-counter'')" \
                          " of  the  next  stack frame is invalid" \
                          " (e.g., not properly aligned)."

#define MY_UNW_EBADFRAME "The next stack frame is invalid."

#define MY_UNW_ESTOPUNWIND "Returned if a call to find_proc_info()" \
                           " returned -UNW_ESTOPUNWIND."

static unw_addr_space_t addr_space;

static struct UPT_info *info;

void init_libunwind(void)
{
    addr_space = unw_create_addr_space(&_UPT_accessors, 0);

    if (!addr_space)
    {
        warnx("libunwind address-space creation failed");

        return;
    }

    // If it fails, we don't care: it will just be slower
    unw_set_caching_policy(addr_space, UNW_CACHE_GLOBAL); 

    info = _UPT_create(g_pid);

    if (!info)
        err(1, "Memory exhausted.");
}

static void print_error(int ret)
{
    if (ret == -UNW_EINVAL)
        warn(MY_UNW_EINVAL);

    else if (ret == -UNW_EUNSPEC)
        warn(MY_UNW_EUNSPEC);

    else if (ret == -UNW_EBADREG)
        warn(MY_UNW_EBADREG);

    else if (ret == -UNW_ENOINFO)
        warn(MY_UNW_ENOINFO);

    else if (ret == -UNW_EBADVERSION)
        warn(MY_UNW_EBADVERSION);

    else if (ret == -UNW_EINVALIDIP)
        warn(MY_UNW_EINVALIDIP);

    else if (ret == -UNW_EBADFRAME)
        warn(MY_UNW_EBADFRAME);

    else if (ret == -UNW_ESTOPUNWIND)
        warn(MY_UNW_ESTOPUNWIND);
}

char *get_binary_file(struct my_mem_mapping **mappings, void *addr)
{
    if (!mappings)
        return NULL;

    struct my_mem_mapping *mapping = find_mem_mappings(mappings, addr);

    if (!mapping)
        return NULL;

    if (mapping->objfile[0] != '/')
        return NULL;

    char path[32];

    // Silence any sprintf error
    if (sprintf(path, "/proc/%d/exe", g_pid) <= 10)
        return NULL;

    struct stat sb;

    struct stat my_sb;

    // Silence any stat error
    if (stat(mapping->objfile, &sb) == -1
        || stat(path, &my_sb) == -1)
        return NULL;

    // Ignore our own binary
    if (sb.st_ino == my_sb.st_ino)
        return NULL;

    return mapping->objfile;
}

#define MY_BT_ERROR(Ret) { \
                             print_error(Ret); \
                             free_mem_mappings(mappings); \
                             return; \
                         }

void print_backtrace(void)
{
    if (!addr_space)
    {
        warnx("libunwind instantiation failed:"
              " impossible to gather a backtrace");

        return;
    }

    struct my_mem_mapping **mappings = get_mem_mappings();

    unw_cursor_t cursor;

    int ret = unw_init_remote(&cursor, addr_space, info);

    if (ret)
        MY_BT_ERROR(ret);

    int step = 1;

    size_t count = 0;

    for (; step > 0 && count < MY_MAX_BT_DEPTH;
         step = unw_step(&cursor), ++count) {
        unw_word_t ip;

        ret = unw_get_reg(&cursor, UNW_REG_IP, &ip);

        if (ret)
            MY_BT_ERROR(ret);

        char buf[32];

        unw_word_t offp;

        ret = unw_get_proc_name(&cursor, buf, 31, &offp);

        buf[31] = 0;

        printf("#%-2zu %18p", count, (void *) ip);
       
        if (!ret || ret == UNW_ENOMEM)
            printf(" in %s%s", buf, ret == UNW_ENOMEM ? "..." : "");

        char *path = get_binary_file(mappings, (void *) ip);

        if (path)
            printf(" from %s", path);

        printf("\n");
    }

    if (!count)
        warnx("No stack");

    if (step < 0)
        MY_BT_ERROR(step);

    free_mem_mappings(mappings);
}

void destroy_libunwind(void)
{
    if (info)
        _UPT_destroy(info);

    if (addr_space)
        unw_destroy_addr_space(addr_space);
}
