#ifndef BREAKPOINTS_H
# define BREAKPOINTS_H

# include <stddef.h>

struct my_bp
{
    void *addr;

    size_t word;

    int enabled;
};

struct my_bp *get_breakpoint(void *addr);

int is_breakpoint(void *addr);

void toggle_breakpoint(struct my_bp *bp);

size_t place_breakpoint(void *addr);

#endif /* BREAKPOINTS_H */
