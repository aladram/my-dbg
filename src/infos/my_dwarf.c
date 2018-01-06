#include "my_dwarf.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "exceptions.h"
#include "my_elf.h"
#include "registers.h"
#include "temp_memory_utils.h"

/*
* Support limited to 32-bit DWARFs
*/

// Dwarf line program header
struct my_dw_lhdr
{
    uint32_t unit_length;

    uint16_t version;

    uint32_t header_length;

    uint8_t minimum_instruction_length;

    /*uint8_t maximum_operations_per_instruction;*/

    uint8_t default_is_stmt;

    int8_t line_base;

    uint8_t line_range;

    uint8_t opcode_base;

    /* uint8_t standard_opcode_lengths[opcode_base - 1]; */ 

    /* char *include_directories[]; */

    /* file_names: complicated format... */
} __attribute__ ((packed));

/*static*/ uint64_t read_leb128(uint8_t **ptr)
{
    uint64_t result = 0;

    for (unsigned shift = 0;; shift += 7)
    {
         uint8_t byte = *((*ptr)++);

         result |= ((byte << 1) >> 1) << shift;

         if (!(byte >> 7))
              break;
    }

    return result;
}

// Dwarf state machine structure
struct my_dw_sm
{
    unsigned address;

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

    // Length: lhdr->opcode_base - 1
    uint8_t *standard_opcode_lengths;

    // Null terminated
    char **include_directories;

    // Null terminated
    struct my_dw_file **files;
};

/*static*/ struct my_dw_sm *init_machine(void)
{
    struct my_dw_sm *sm = tmp_calloc(1, sizeof(*sm));

    sm->file = 1;

    sm->line = 1;

    return sm;
}

static size_t add_string(char ***array, char *str, size_t length)
{
    *array = tmp_realloc(*array, sizeof(char *) * length,
                         sizeof(char *) * (length + 1));

    (*array)[length] = str;

    return length + 1;
}

static struct my_dw_file **store_files(char *str)
{
    struct my_dw_file **files = tmp_malloc(sizeof(*files));

    size_t length = 0;

    while (*str)
    {
        files = tmp_realloc(files, sizeof(*files) * (length + 1),
                            sizeof(*files) * (length + 2));

        struct my_dw_file *f = tmp_malloc(sizeof(*f));

        files[length++] = f;

        f->path = str;

        str += strlen(str) + 1;

        f->index = read_leb128((uint8_t **) &str);

        f->last_modification = read_leb128((uint8_t **) &str);

        f->file_size = read_leb128((uint8_t **) &str);
    }

    files[length] = NULL;

    return files;
}

static struct my_dw_lconf *dwarf_lines_config(void *s_addr)
{
    struct my_dw_lconf *lconf = tmp_calloc(1, sizeof(*lconf));

    lconf->lhdr = s_addr;

    lconf->standard_opcode_lengths = (void *) (lconf->lhdr + 1);

    size_t length = 0;

    char *str = (char *) (lconf->standard_opcode_lengths
                          + lconf->lhdr->opcode_base - 1);

    for (; *str; str += strlen(str) + 1)
        length = add_string(&lconf->include_directories, str, length);

    ++str;

    add_string(&lconf->include_directories, NULL, length);

    lconf->files = store_files(str);

    return lconf;
}

#include <assert.h>
static void print_line_at_addr(struct my_elf *elf, void *addr)
{
    // HARDCODING, please use DW_AT_stmt_list
    // size_t offset = 0;

    (void) addr;

    struct my_elf_section *s = elf_section(elf, ".debug_line", SHT_PROGBITS);

    struct my_dw_lconf *lconf = dwarf_lines_config(s->addr);

    assert(0 && "Not implemented" && lconf);
}

void print_line(void)
{
    void *addr = (void *) get_register(MY_REG_RIP);

    enum my_exception my_ex = None;

    struct my_elf *elf = NULL;

    try
    {
        elf = open_elf();

        print_line_at_addr(elf, addr);
    }
    finally
    {
        my_ex = ex;
    }
    etry;

    try
    {
        close_elf(elf);
    }
    catch (IOException)
    {
        if (my_ex == None)
            my_ex = ex;
    }
    etry;

    if (my_ex != None)
        throw(my_ex);
}
