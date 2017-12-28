#include "capstone_wrapper.h"

#include <capstone/capstone.h>
#include <err.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "memory_utils.h"
#include "registers.h"

struct my_instr *get_instruction(void *addr)
{
    char *mem = read_memory(addr, 16);

    if (!mem)
        return NULL;

    csh handle;

    cs_insn *insn;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
    {
        warnx("Dissasemble failed");

        return NULL;
    }

    cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);

    size_t count = cs_disasm(handle, (uint8_t *) mem, 16, 0, 1, &insn);

    if (!count)
    {
        warnx("Dissasemble failed");

        cs_close(&handle);

        return NULL;
    }

    struct my_instr *instr = my_malloc(sizeof(*instr));

    instr->size = insn[0].size;

    if (asprintf(&instr->str, "%s%s\n", insn[0].mnemonic, insn[0].op_str) == -1)
    {
        warnx("Dissasemble failed");

        free(instr);

        cs_close(&handle);

        return NULL;
    }

    cs_free(insn, count);

    cs_close(&handle);

    return instr;
}

struct my_instr *get_current_instruction(void)
{
    void *addr = (void *) get_register(MY_REG_RIP);

    if (!addr)
    {
        warnx("Dissasemble failed");

        return NULL;
    }

    return get_instruction(addr);
}
