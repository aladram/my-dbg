#include "registers.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "binary.h"
#include "commands.h"


#define CASE_REG(RegUp, RegLow) case RegUp: \
                                    return (size_t *) &regs->RegLow;

static size_t *get_register_internal(struct user_regs_struct *regs,
                                     enum my_reg reg)
{
    switch (reg)
    {
    CASE_REG(MY_REG_RIP, rip);
    CASE_REG(MY_REG_RSP, rsp);
    CASE_REG(MY_REG_RBP, rbp);
    CASE_REG(MY_REG_EFLAGS, eflags);
    CASE_REG(MY_REG_ORIG_RAX, orig_rax);
    CASE_REG(MY_REG_RAX, rax);
    CASE_REG(MY_REG_RBX, rbx);
    CASE_REG(MY_REG_RCX, rcx);
    CASE_REG(MY_REG_RDX, rdx);
    CASE_REG(MY_REG_RDI, rdi);
    CASE_REG(MY_REG_RSI, rsi);
    CASE_REG(MY_REG_R8, r8);
    CASE_REG(MY_REG_R9, r9);
    CASE_REG(MY_REG_R10, r10);
    CASE_REG(MY_REG_R11, r11);
    CASE_REG(MY_REG_R12, r12);
    CASE_REG(MY_REG_R13, r13);
    CASE_REG(MY_REG_R14, r14);
    CASE_REG(MY_REG_R15, r15);
    CASE_REG(MY_REG_CS, cs);
    CASE_REG(MY_REG_DS, ds);
    CASE_REG(MY_REG_ES, es);
    CASE_REG(MY_REG_FS, fs);
    CASE_REG(MY_REG_GS, gs);
    CASE_REG(MY_REG_SS, ss);
    CASE_REG(MY_REG_FS_BASE, fs_base);
    CASE_REG(MY_REG_GS_BASE, gs_base);
    }

    return 0;
}

int get_registers(struct user_regs_struct *regs)
{
    if (ptrace(PTRACE_GETREGS, g_pid, NULL, regs) == -1)
    {
        warn("ptrace failed");

        return 0;
    }

    return 1;
}

size_t get_register(enum my_reg reg)
{
    struct user_regs_struct regs;

    if (!get_registers(&regs))
        return 0;

    return *get_register_internal(&regs, reg);
}

void set_register(enum my_reg reg, size_t value)
{
    struct user_regs_struct regs;

    if (!get_registers(&regs))
        return;

    size_t *reg_ptr = get_register_internal(&regs, reg);

    *reg_ptr = value;

    if (ptrace(PTRACE_SETREGS, g_pid, NULL, &regs) == -1)
        warn("ptrace failed");
}
