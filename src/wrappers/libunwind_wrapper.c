#include "libunwind_wrapper.h"

#include <err.h>
#include <libunwind.h>
#include <libunwind-ptrace.h>
#include <stdio.h>

#include "binary.h"

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

void print_backtrace(void)
{
    if (!addr_space)
    {
        warnx("libunwind instantiation failed:"
              " impossible to gather a backtrace");

        return;
    }

    unw_cursor_t cursor;

    int ret = unw_init_remote(&cursor, addr_space, info);

    if (ret)
    {
        print_error(ret);

        return;
    }

    unw_word_t ip;

    unw_word_t sp;

    int step = unw_step(&cursor);

    for (; step > 0; step = unw_step(&cursor)) {
        ret = unw_get_reg(&cursor, UNW_REG_IP, &ip);

        if (ret)
        {
            print_error(ret);

            return;
        }

        ret = unw_get_reg(&cursor, UNW_REG_SP, &sp);

        if (ret)
        {
            print_error(ret);

            return;
        }

        printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
    }

    if (step < 0)
        print_error(step);
}

void destroy_libunwind(void)
{
    _UPT_destroy(info);

    unw_destroy_addr_space(addr_space);
}
