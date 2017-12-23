#include "breakpoints.h"

#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>

#include "binary.h"
#include "commands.h"
#include "memory_utils.h"

static struct my_bp *breakpoints;

static size_t bp_len;

struct my_bp *get_breakpoint(void *addr)
{
    for (size_t i = 0; i < bp_len; ++i)
        if (breakpoints[i].addr == addr && breakpoints[i].enabled)
            return breakpoints + i;

    return NULL;
}

int is_breakpoint(void *addr)
{
    return get_breakpoint(addr) ? 1 : 0;
}

void toggle_breakpoint(struct my_bp *bp)
{
    errno = 0;

    size_t word = ptrace(PTRACE_PEEKDATA, g_pid, bp->addr, NULL);

    if (errno)
        goto error;

    word = (word & (~0xFF));

    if (!bp->enabled)
        word |= 0xCC;

    else
        word |= bp->word & 0xFF;

    if (ptrace(PTRACE_POKEDATA, g_pid, bp->addr, (void *) word) == -1)
        goto error;

    bp->enabled = !bp->enabled;

    return;

error:
    warn("ptrace failed");
}

size_t place_breakpoint(void *addr)
{
    errno = 0;
    
    size_t word = ptrace(PTRACE_PEEKDATA, g_pid, addr, NULL);

    if (errno)
        goto error;

    size_t word_bp = (word & (~0xFF)) | 0xCC;

    if (ptrace(PTRACE_POKEDATA, g_pid, addr, (void *) word_bp) == -1)
        goto error;

    breakpoints = my_realloc(breakpoints, ++bp_len * sizeof(struct my_bp));

    breakpoints[bp_len - 1].addr = addr;

    breakpoints[bp_len - 1].word = word;

    breakpoints[bp_len - 1].enabled = 1;

    return bp_len;

error:
    warn("ptrace failed");

    return 0;
}
