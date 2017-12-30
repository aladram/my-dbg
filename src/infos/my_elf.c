#include "my_elf.h"

#include <elf.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "exceptions.h"
#include "file_utils.h"
#include "format_utils.h"
#include "memory_utils.h"

static FILE *open_elf(void)
{
    char *path = get_proc_path("exe");

    FILE *f = fopen(path, "r");

    if (!f)
    {
        warn("%s", path);

        throw(IOException);
    }

    return f;
}

static void check_header_integrity(Elf64_Ehdr *header)
{
    unsigned char magic[4] = {
        ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3
    };

    if (memcmp(header->e_ident, magic, 4))
        throw(ELFException);
}

/*
* File cursor should be at 0 before calling this function
*/
static Elf64_Ehdr *elf_header(FILE *f)
{
    Elf64_Ehdr *header = my_malloc(sizeof(*header));

    if (file_size(f) < sizeof(*header))
        throw(ELFException);

    if (fread(header, sizeof(*header), 1, f) != 1)
        throw(IOException);

    check_header_integrity(header);

    return header;
}

static Elf64_Shdr *elf_sections_header(FILE *f, Elf64_Ehdr *header)
{
    uint16_t shnum = header->e_shnum;

    /*
    * No use of header->e_shentsize for now: only supporting 64bit binaries,
    * where header->e_shentsize = 64 bytes
    *
    * so... TODO: support 32bit binaries
    */
    Elf64_Shdr *s_header = my_malloc(sizeof(*s_header) * shnum);

    if (fseek(f, header->e_shoff, SEEK_SET) == -1)
        throw(IOException);

    if (fread(s_header, sizeof(*s_header), shnum, f) != shnum)
        throw(ELFException);

    return s_header;
}

void *get_address(char *function)
{
    FILE *f = open_elf();

    Elf64_Ehdr *header = elf_header(f);

    Elf64_Shdr *s_header = elf_sections_header(f, header);

    (void)s_header;

    (void)function;

    return NULL;
}
