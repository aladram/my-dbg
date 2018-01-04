#include "registers.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/user.h>

#include "binary.h"
#include "commands.h"
#include "exceptions.h"
#include "my_syscalls.h"

#include "registers_list.h"

#define CASE_REG(RegUp, RegLow) if (reg == RegUp) \
                                    return (size_t *) &regs->RegLow;

static size_t *get_register_internal(struct user_regs_struct *regs,
                                     enum my_reg reg)
{
CASES_REG

    return 0;
}

void get_registers(struct user_regs_struct *regs)
{
    my_ptrace(PTRACE_GETREGS, NULL, regs);
}

size_t get_register(enum my_reg reg)
{
    struct user_regs_struct regs;

    get_registers(&regs);

    return *get_register_internal(&regs, reg);
}

void set_register(enum my_reg reg, size_t value)
{
    struct user_regs_struct regs;

    get_registers(&regs);

    size_t *reg_ptr = get_register_internal(&regs, reg);

    *reg_ptr = value;

    my_ptrace(PTRACE_SETREGS, NULL, &regs);
}

#undef CASE_REG
#define CASE_REG(RegUp, RegLow) if (!strcmp(reg, #RegLow)) \
                                    return get_register(RegUp);

size_t get_register_from_name(char *reg)
{
CASES_REG

    warnx("Unknown register '%s'", reg);

    throw(Exception);

    return 0;
}
