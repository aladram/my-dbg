#ifndef BREAKPOINTS_H
# define BREAKPOINTS_H

# include <stddef.h>

enum my_bp_flags
{
    MY_BP_TEMP     = 1,
    MY_BP_INTERNAL = 2,
    MY_BP_SYSCALL  = 4
};

struct my_bp
{
    size_t id;

    enum my_bp_flags flags;

    void *addr;

    size_t word;

    int enabled;

    int deleted;
};

struct my_bp *g_breakpoints;

size_t g_bp_len;

struct my_bp *get_breakpoint(void *addr);

int is_breakpoint(void *addr);

void toggle_breakpoint(struct my_bp *bp);

size_t place_breakpoint(void *addr, enum my_bp_flags flags);

#endif /* BREAKPOINTS_H */
