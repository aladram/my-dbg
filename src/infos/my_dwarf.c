#include "my_dwarf.h"

#include <err.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dwarf_state_machine.h"
#include "exceptions.h"
#include "file_utils.h"
#include "format_utils.h"
#include "my_elf.h"
#include "registers.h"
#include "temp_memory_utils.h"

/*
* Support limited to 32-bit DWARFs
* and versions 2, 3 and 4 of DWARF standard
*/

uint64_t read_leb128(uint8_t **ptr)
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

    lconf->lhdrx = (void *) ((struct my_dw_lhdr *) lconf->lhdr + 1);

    if (lconf->lhdr->version < 4)
        lconf->standard_opcode_lengths = (uint8_t *) ((struct my_dw_lhdr2 *)
                                                      lconf->lhdrx + 1) - 1;

    else
        lconf->standard_opcode_lengths = (uint8_t *) ((struct my_dw_lhdr4 *)
                                                      lconf->lhdrx + 1);

    size_t length = 0;

    char *str = (char *) (lconf->standard_opcode_lengths
                          + VAL(opcode_base) - 1);

    for (; *str; str += strlen(str) + 1)
        length = add_string(&lconf->include_directories, str, length);

    ++str;

    add_string(&lconf->include_directories, NULL, length);

    lconf->files = store_files(str);

    return lconf;
}

void reset_machine(struct my_dw_lconf *lconf, struct my_dw_sm *sm)
{
    memset(sm, 0, sizeof(*sm));

    sm->file = 1;

    sm->line = 1;

    sm->is_stmt = VAL(default_is_stmt) ? true : false;
}

static struct my_dw_sm *init_machine(struct my_dw_lconf *lconf)
{
    struct my_dw_sm *sm = tmp_malloc(sizeof(*sm));

    reset_machine(lconf, sm);

    return sm;
}

struct my_dw_lines
{
    struct my_dw_lconf *lconf;

    struct my_dw_sm **states;
};

static struct my_dw_lines *dwarf_lines(void *s_addr)
{
    struct my_dw_lines *lines = tmp_malloc(sizeof(*lines));

    lines->lconf = dwarf_lines_config(s_addr);

    struct my_dw_sm *sm = init_machine(lines->lconf);

    lines->states = run_machine(lines->lconf, sm);

    return lines;
}

static long addr_dist(void *addr1, void *addr2)
{
    long diff = (char *) addr1 - (char *) addr2;

    if (diff <= 0)
        return -diff;

    return LONG_MAX;
}

static struct my_dw_sm *get_nearest_state(struct my_dw_lines *lines,
                                          void *addr, void *base_addr)
{
    long dist = LONG_MAX;

    struct my_dw_sm *my_sm = NULL;

    for (size_t i = 0; lines->states[i]; ++i)
    {
        struct my_dw_sm *sm = lines->states[i];

        long d = addr_dist((char *) base_addr + (size_t) sm->address, addr);

        if (d == LONG_MAX)
            continue;

        if (d <= dist)
        {
            dist = d;

            my_sm = sm;
        }
    }

    return my_sm;
}

static void print_line_at_addr(struct my_elf *elf, void *addr)
{
    // HARDCODING, please use DW_AT_stmt_list
    // size_t offset = 0;

    void *base_addr = calc_base_address(elf->elf, NULL);

    struct my_elf_section *s = elf_section(elf, ".debug_line", SHT_PROGBITS);

    struct my_dw_lines *lines = dwarf_lines(s->addr);

    struct my_dw_sm *my_sm = get_nearest_state(lines, addr, base_addr);

    if (!my_sm)
    {
        warnx("No line found: no debugging informations?");

        return;
    }

    if (!my_sm->file)
        throw(DWARFException);

    struct my_dw_file *file = lines->lconf->files[my_sm->file - 1];

    char *dir;

    if (!file->path)
        throw(DWARFException);

    if (file->path[0] == '/')
        dir = NULL;
    
    if (!file->index)
        dir = real_path(get_proc_path("exe"));

    // TODO: check include_directories array length
    else
        dir = lines->lconf->include_directories[file->index - 1];

    print_file_line(dir, file->path, my_sm->line);
}

static void get_line_address_internal(struct my_elf *elf,
                                      char *file, unsigned line)
{
    (void) elf;

    (void) file;

    (void) line;
}

#define SAFE_DWARF(Code) \
    enum my_exception my_ex = None; \
 \
    struct my_elf *elf = NULL; \
 \
    try \
    { \
        elf = open_elf(); \
 \
        Code; \
    } \
    finally \
    { \
        my_ex = ex; \
    } \
    etry; \
 \
    try \
    { \
        close_elf(elf); \
    } \
    catch (IOException) \
    { \
        if (my_ex == None) \
            my_ex = ex; \
    } \
    etry; \
 \
    if (my_ex != None) \
        throw(my_ex)

void print_line(void)
{
    void *addr = (void *) get_register(MY_REG_RIP);

    SAFE_DWARF(print_line_at_addr(elf, addr));
}

void *get_line_address(char *file, unsigned line)
{
    void *addr = NULL;

    SAFE_DWARF(get_line_address_internal(elf, file, line));

    return addr;
}
