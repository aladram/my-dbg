#ifndef REGISTERS_LIST_H
# define REGISTERS_LIST_H

#define CASES_REG \
    CASE_REG(MY_REG_RIP, rip); \
    CASE_REG(MY_REG_RSP, rsp); \
    CASE_REG(MY_REG_RBP, rbp); \
    CASE_REG(MY_REG_EFLAGS, eflags); \
    CASE_REG(MY_REG_ORIG_RAX, orig_rax); \
    CASE_REG(MY_REG_RAX, rax); \
    CASE_REG(MY_REG_RBX, rbx); \
    CASE_REG(MY_REG_RCX, rcx); \
    CASE_REG(MY_REG_RDX, rdx); \
    CASE_REG(MY_REG_RDI, rdi); \
    CASE_REG(MY_REG_RSI, rsi); \
    CASE_REG(MY_REG_R8, r8); \
    CASE_REG(MY_REG_R9, r9); \
    CASE_REG(MY_REG_R10, r10); \
    CASE_REG(MY_REG_R11, r11); \
    CASE_REG(MY_REG_R12, r12); \
    CASE_REG(MY_REG_R13, r13); \
    CASE_REG(MY_REG_R14, r14); \
    CASE_REG(MY_REG_R15, r15); \
    CASE_REG(MY_REG_CS, cs); \
    CASE_REG(MY_REG_DS, ds); \
    CASE_REG(MY_REG_ES, es); \
    CASE_REG(MY_REG_FS, fs); \
    CASE_REG(MY_REG_GS, gs); \
    CASE_REG(MY_REG_SS, ss); \
    CASE_REG(MY_REG_FS_BASE, fs_base); \
    CASE_REG(MY_REG_GS_BASE, gs_base);

#endif /* REGISTERS_LIST_H */
