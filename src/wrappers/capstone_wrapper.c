#include "capstone_wrapper.h"

#include <capstone/capstone.h>
#include <err.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "exceptions.h"
#include "memory.h"
#include "registers.h"
#include "temp_memory_utils.h"

struct my_instr *get_instruction(void *addr)
{
    char *mem = read_memory(addr, 16);

    csh handle;

    cs_insn *insn;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        throw(DisasmException);

    cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);

    size_t count = cs_disasm(handle, (uint8_t *) mem, 16, 0, 1, &insn);

    if (!count)
    {
        cs_close(&handle);

        throw(DisasmException);
    }

    struct my_instr *instr = tmp_malloc(sizeof(*instr));

    instr->size = insn[0].size;

    if (asprintf(&instr->str, "%s%s\n", insn[0].mnemonic, insn[0].op_str) == -1)
    {
        cs_free(insn, count);

        cs_close(&handle);

        throw(DisasmException);
    }

    cs_free(insn, count);

    cs_close(&handle);

    char *old_str = instr->str;

    instr->str = tmp_strdup(old_str);

    free(old_str);

    return instr;
}

struct my_instr *get_current_instruction(void)
{
    void *addr = (void *) get_register(MY_REG_RIP);

    return get_instruction(addr);
}
