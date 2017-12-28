#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/user.h>

#include "binary.h"
#include "commands.h"
#include "errors.h"
#include "exceptions.h"
#include "registers.h"

static void print_register(char *name, unsigned long long int reg)
{
    printf("%s: 0x%llx\n", name, reg);
}

#define PRINT_REG(Regs, Reg) print_register(#Reg, Regs->Reg)

static void print_registers(struct user_regs_struct *regs)
{
    PRINT_REG(regs, rip);
    PRINT_REG(regs, rsp);
    PRINT_REG(regs, rbp);
    PRINT_REG(regs, eflags);
    PRINT_REG(regs, orig_rax);
    PRINT_REG(regs, rax);
    PRINT_REG(regs, rbx);
    PRINT_REG(regs, rcx);
    PRINT_REG(regs, rdx);
    PRINT_REG(regs, rdi);
    PRINT_REG(regs, rsi);
    PRINT_REG(regs, r8);
    PRINT_REG(regs, r9);
    PRINT_REG(regs, r10);
    PRINT_REG(regs, r11);
    PRINT_REG(regs, r12);
    PRINT_REG(regs, r13);
    PRINT_REG(regs, r14);
    PRINT_REG(regs, r15);
    PRINT_REG(regs, cs);
    PRINT_REG(regs, ds);
    PRINT_REG(regs, es);
    PRINT_REG(regs, fs);
    PRINT_REG(regs, gs);
    PRINT_REG(regs, ss);
    PRINT_REG(regs, fs_base);
    PRINT_REG(regs, gs_base);
}

static void cmd_info_regs(size_t argc, char **argv)
{
    (void)argc;

    (void)argv;

    struct user_regs_struct regs;

    try
    {
        get_registers(&regs);

        print_registers(&regs);
    }
    catch (TraceException)
    {
        ptrace_error();
    }
    etry;
}

register_command(info_regs,
                 cmd_info_regs,
                 "Display registers",
                 "info_regs");
