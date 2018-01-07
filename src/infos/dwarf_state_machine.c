#include "dwarf_state_machine.h"

#include <err.h>
#include <stdint.h>
#include <string.h>

#include "exceptions.h"
#include "my_dwarf.h"
#include "temp_memory_utils.h"

#define DW_LNS_copy                 0x01
#define DW_LNS_advance_pc           0x02
#define DW_LNS_advance_line         0x03
#define DW_LNS_set_file             0x04
#define DW_LNS_set_column           0x05
#define DW_LNS_negate_stmt          0x06
#define DW_LNS_set_basic_block      0x07
#define DW_LNS_const_add_pc         0x08
#define DW_LNS_fixed_advance_pc     0x09
#define DW_LNS_set_prologue_end     0x0a
#define DW_LNS_set_epilogue_begin   0x0b
#define DW_LNS_set_isa              0x0c

#define DW_LNE_end_sequence         0x01
#define DW_LNE_set_address          0x02
#define DW_LNE_define_file          0x03
#define DW_LNE_set_discriminator    0x04
#define DW_LNE_lo_user              0x80
#define DW_LNE_hi_user              0xff

static void clone_state(struct my_dw_sm *sm,
                        struct my_dw_sm ***states,
                        size_t *length)
{
    *states = tmp_realloc(*states, sizeof(sm) * (*length),
                          sizeof(sm) * (*length + 1));

    struct my_dw_sm *state = tmp_malloc(sizeof(*state));

    (*states)[*length] = state;

    ++(*length);

    memcpy(state, sm, sizeof(*sm));
}

static void advance_addr_and_op_index(struct my_dw_lconf *lconf,
                                      struct my_dw_sm *sm,
                                      uint64_t op_adv)
{
    sm->address += (VAL(minimum_instruction_length)
                    * ((sm->op_index + op_adv)
                       / VAL(maximum_operations_per_instruction)));

    sm->op_index = ((sm->op_index + op_adv)
                    % VAL(maximum_operations_per_instruction));
}

static void extended_instruction(struct my_dw_lconf *lconf,
                                 struct my_dw_sm *sm,
                                 struct my_dw_sm ***states,
                                 size_t *length,
                                 uint8_t **ptr)
{
    (*ptr)++;

    /*uint64_t len =*/ read_leb128(ptr);

    uint8_t opcode = *((*ptr)++);

#ifdef MY_DEBUG
    warnx("Extended instruction: 0x%02hhu", opcode);
#endif

    if (opcode == DW_LNE_end_sequence)
    {
        sm->end_sequence = true;

        clone_state(sm, states, length);

        reset_machine(lconf, sm);
    }

    else if (opcode == DW_LNE_set_address)
    {
        sm->address = *((void **) (*ptr));

        *ptr += sizeof(void *);

        sm->op_index = 0;
    }

    else if (opcode == DW_LNE_define_file)
    {
        warnx("Unsupported operation: 0x%02hhx", opcode);

        throw(DWARFException);
    }

    else if (opcode == DW_LNE_set_discriminator)
        sm->discriminator = read_leb128(ptr);

    else
    {
        warnx("Invalid extended instruction: 0x%02hhx", opcode);

        throw(DWARFException);
    }
}

static void standard_instruction(struct my_dw_lconf *lconf,
                                 struct my_dw_sm *sm,
                                 struct my_dw_sm ***states,
                                 size_t *length,
                                 uint8_t **ptr)
{
    (void)lconf;

    uint8_t opcode = *((*ptr)++);

    if (opcode == DW_LNS_copy)
    {
        clone_state(sm, states, length);

        sm->discriminator = 0;

        sm->basic_block = false;

        sm->prologue_end = false;

        sm->epilogue_begin = false;
    }

    else if (opcode == DW_LNS_advance_pc)
        advance_addr_and_op_index(lconf, sm, read_leb128(ptr));

    else if (opcode == DW_LNS_advance_line)
        sm->line += read_leb128(ptr);

    else if (opcode == DW_LNS_set_file)
        sm->file = read_leb128(ptr);

    else if (opcode == DW_LNS_set_column)
        sm->column = read_leb128(ptr);

    else if (opcode == DW_LNS_negate_stmt)
        sm->is_stmt = !sm->is_stmt;

    else if (opcode == DW_LNS_set_basic_block)
        sm->basic_block = true;

    else if (opcode == DW_LNS_const_add_pc)
        advance_addr_and_op_index(lconf, sm,
                                  (255 - VAL(opcode_base)) / VAL(line_range));

    else if (opcode == DW_LNS_fixed_advance_pc)
    {
        sm->address += *((uint16_t *) (*ptr));

        *ptr += sizeof(uint16_t);

        sm->op_index = 0;
    }

    else if (opcode == DW_LNS_set_prologue_end)
        sm->prologue_end = true;

    else if (opcode == DW_LNS_set_epilogue_begin)
        sm->epilogue_begin = true;

    else if (opcode == DW_LNS_set_isa)
        sm->isa = read_leb128(ptr);

    else
    {
        warnx("Invalid standard instruction: 0x%02hhx", opcode);

        throw(DWARFException);
    }
}

/*
* cf. section 6.2.5.1, Special Opcodes (DWARF4.pdf)
*/
static void special_instruction(struct my_dw_lconf *lconf,
                                struct my_dw_sm *sm,
                                struct my_dw_sm ***states,
                                size_t *length,
                                uint8_t opcode)
{
    uint8_t adj_opcode = opcode - VAL(opcode_base);

    uint8_t op_adv = adj_opcode / VAL(line_range);

    // 1.
    sm->line += VAL(line_base) + (adj_opcode % VAL(line_range));

    // 2.
    advance_addr_and_op_index(lconf, sm, op_adv);

    // 3.
    clone_state(sm, states, length);

    // 4.
    sm->basic_block = false;

    // 5.
    sm->prologue_end = false;

    // 6.
    sm->epilogue_begin = false;

    // 7.
    sm->discriminator = 0;
}

struct my_dw_sm **run_machine(struct my_dw_lconf *lconf,
                              struct my_dw_sm *sm)
{
    uint8_t *ptr = ((uint8_t *) (lconf->lhdr + 1)
                    + sizeof(uint32_t) + VAL(header_length));

    size_t size = lconf->lhdr->unit_length - VAL(header_length) - 6;

    struct my_dw_sm **states = NULL;

    size_t length = 0;

    for (size_t i = 0; i < size;)
    {
        uint8_t *bck = ptr;

        if (!*ptr)
            extended_instruction(lconf, sm, &states, &length, &ptr);

        else if (*ptr < VAL(opcode_base))
        {
#ifdef MY_DEBUG
            warnx("Standard instruction: 0x%02hhu", *ptr);
#endif

            standard_instruction(lconf, sm, &states, &length, &ptr);
        }

        else
        {
#ifdef MY_DEBUG
            warnx("Special instruction: 0x%02hhu", *ptr - VAL(opcode_base));
#endif

            special_instruction(lconf, sm, &states, &length, *(ptr++));
        }

#ifdef MY_DEBUG
        warnx("Instruction total length: %ld", ptr - bck);
#endif

        i += ptr - bck;
    }

    states = tmp_realloc(states, sizeof(sm) * length,
                         sizeof(sm) * (length + 1));

    states[length] = NULL;

    return states;
}
