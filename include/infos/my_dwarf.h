#ifndef MY_DWARF_H
# define MY_DWARF_H

# include <stdbool.h>
# include <stdint.h>

/*
* DWARF line program header structures
*
* Disabled code only here as a memo (not a valid C code)
*/

struct my_dw_lhdr
{
    uint32_t unit_length;

    uint16_t version;
} __attribute__ ((packed));

struct my_dw_lhdr2
{
    uint32_t header_length;

    uint8_t minimum_instruction_length;

    uint8_t default_is_stmt;

    int8_t line_base;

    uint8_t line_range;

    uint8_t opcode_base;

#if 0
    uint8_t standard_opcode_lengths[opcode_base - 1];

    char include_directories[][];

    struct my_dw_file files[];
#endif

    /* Used to ignore compiler errors */
    uint8_t maximum_operations_per_instruction;
} __attribute__ ((packed));

struct my_dw_lhdr4
{
    uint32_t header_length;

    uint8_t minimum_instruction_length;

    uint8_t maximum_operations_per_instruction;

    uint8_t default_is_stmt;

    int8_t line_base;

    uint8_t line_range;

    uint8_t opcode_base;

#if 0
    uint8_t standard_opcode_lengths[opcode_base - 1];

    char include_directories[][];

    struct my_dw_file files[];
#endif
} __attribute__ ((packed));

/*
* DWARF state machine structure
*
* Program ran will produce a matrix of state machine structures
*/
struct my_dw_sm
{
    char *address;

    unsigned op_index;

    unsigned file;

    unsigned line;

    unsigned column;

    bool is_stmt;

    bool basic_block;

    bool end_sequence;

    bool prologue_end;

    bool epilogue_begin;

    unsigned isa;

    unsigned discriminator;
};

struct my_dw_file
{
    char *path;

    uint64_t index;

    uint64_t last_modification;

    uint64_t file_size;
};

struct my_dw_lconf
{
    struct my_dw_lhdr *lhdr;

    void *lhdrx;

    // Length: opcode_base - 1
    uint8_t *standard_opcode_lengths;

    // Null terminated
    char **include_directories;

    // Null terminated
    struct my_dw_file **files;
};

#define CONF_VALUE_AUX(Lconf, Key) \
                               ((Lconf->lhdr->version < 4) \
                               ? (((struct my_dw_lhdr2 *) Lconf->lhdrx)->Key) \
                               : (((struct my_dw_lhdr4 *) Lconf->lhdrx)->Key))

#define CONF_VALUE(Lconf, Key) \
           ((Lconf->lhdr->version < 4 \
            && !strcmp(#Key, "maximum_operations_per_instruction")) \
            ? 1 : CONF_VALUE_AUX(Lconf, Key))

#define VAL(Key) CONF_VALUE(lconf, Key)

uint64_t read_leb128(uint8_t **ptr);

void reset_machine(struct my_dw_lconf *lconf, struct my_dw_sm *sm);

void print_line(void);

#endif /* MY_DWARF_H */
