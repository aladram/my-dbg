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
#include "syscalls.h"

struct my_bp *g_breakpoints;

size_t g_bp_len;

struct my_bp *get_breakpoint(void *addr)
{
    for (size_t i = 0; i < g_bp_len; ++i)
    {
        struct my_bp *bp = g_breakpoints + i;

        if (bp->addr == addr && bp->enabled && !bp->deleted
            && !(bp->flags & MY_BP_SYSCALL))
            return bp;
    }

    return NULL;
}

int is_breakpoint(void *addr)
{
    return get_breakpoint(addr) ? 1 : 0;
}

void toggle_breakpoint(struct my_bp *bp)
{
    if (!bp->enabled && (bp->flags & MY_BP_TEMP))
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

size_t place_breakpoint(void *addr, enum my_bp_flags flags)
{
    size_t word;

    if (!(flags & MY_BP_SYSCALL))
    {
        word = my_ptrace(PTRACE_PEEKDATA, addr, NULL);

        size_t word_bp = (word & (~0xFF)) | 0xCC;

        my_ptrace(PTRACE_POKEDATA, addr, (void *) word_bp);
    }

    else
        word = g_syscalls_nb - 1;

    g_breakpoints = my_realloc(g_breakpoints,
                               ++g_bp_len * sizeof(struct my_bp));

    g_breakpoints[g_bp_len - 1].id = g_bp_len;

    g_breakpoints[g_bp_len - 1].flags = flags;

    g_breakpoints[g_bp_len - 1].addr = addr;

    g_breakpoints[g_bp_len - 1].word = word;

    g_breakpoints[g_bp_len - 1].enabled = 1;

    g_breakpoints[g_bp_len - 1].deleted = 0;

    return g_bp_len;
}
