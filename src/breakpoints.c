#include "breakpoints.h"

#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "binary.h"
#include "commands.h"
#include "memory_utils.h"
#include "my_syscalls.h"

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
    if (!bp->enabled && bp->temp)
        return;

    size_t word = my_ptrace(PTRACE_PEEKDATA, bp->addr, NULL);

    word = (word & (~0xFF));

    if (!bp->enabled)
        word |= 0xCC;

    else
        word |= bp->word & 0xFF;

    my_ptrace(PTRACE_POKEDATA, bp->addr, (void *) word);

    bp->enabled = !bp->enabled;
}

size_t place_breakpoint(void *addr, int temp)
{
    size_t word = my_ptrace(PTRACE_PEEKDATA, addr, NULL);

    size_t word_bp = (word & (~0xFF)) | 0xCC;

    my_ptrace(PTRACE_POKEDATA, addr, (void *) word_bp);

    breakpoints = my_realloc(breakpoints, ++bp_len * sizeof(struct my_bp));

    breakpoints[bp_len - 1].id = bp_len;

    breakpoints[bp_len - 1].addr = addr;

    breakpoints[bp_len - 1].word = word;

    breakpoints[bp_len - 1].enabled = 1;

    breakpoints[bp_len - 1].temp = temp;

    return bp_len;
}
