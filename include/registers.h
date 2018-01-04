#ifndef REGISTERS_H
# define REGISTERS_H

# include <stddef.h>
# include <sys/user.h>

enum my_reg
{
    MY_REG_RIP,
    MY_REG_RSP,
    MY_REG_RBP,
    MY_REG_EFLAGS,
    MY_REG_ORIG_RAX,
    MY_REG_RAX,
    MY_REG_RBX,
    MY_REG_RCX,
    MY_REG_RDX,
    MY_REG_RDI,
    MY_REG_RSI,
    MY_REG_R8,
    MY_REG_R9,
    MY_REG_R10,
    MY_REG_R11,
    MY_REG_R12,
    MY_REG_R13,
    MY_REG_R14,
    MY_REG_R15,
    MY_REG_CS,
    MY_REG_DS,
    MY_REG_ES,
    MY_REG_FS,
    MY_REG_GS,
    MY_REG_SS,
    MY_REG_FS_BASE,
    MY_REG_GS_BASE
};

void get_registers(struct user_regs_struct *regs);

size_t get_register(enum my_reg reg);

void set_register(enum my_reg reg, size_t value);

size_t get_register_from_name(char *reg);

#endif /* REGISTERS_H */
