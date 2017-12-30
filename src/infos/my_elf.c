#include "my_elf.h"

#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "exceptions.h"
#include "file_utils.h"
#include "format_utils.h"
#include "memory_utils.h"

static size_t elf_size(void)
{
    char *path = get_proc_path("exe");

    size_t size = file_size(path);

    if (size < sizeof(Elf64_Ehdr))
        throw(ELFException);

    return size;
}

static void *open_elf(size_t size)
{
    char *path = get_proc_path("exe");

    int fd = open(path, O_RDONLY);

    if (fd == -1)
    {
        warn("%s", path);

        throw(IOException);
    }

    void *elf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (elf == MAP_FAILED)
    {
        warn("%s", path);

        throw(IOException);
    }

    return elf;
}

static void check_header_integrity(Elf64_Ehdr *header)
{
    unsigned char magic[4] = {
        ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3
    };

    if (memcmp(header->e_ident, magic, 4))
        throw(ELFException);
}

static Elf64_Ehdr *elf_header(void *elf)
{
    Elf64_Ehdr *header = elf;

    check_header_integrity(header);

    return header;
}

static Elf64_Shdr *elf_sections_header(char *elf, Elf64_Ehdr *header)
{
    /*
    * No use of header->e_shentsize for now: only supporting 64bit binaries,
    * where header->e_shentsize = 64 bytes
    *
    * so... TODO: support 32bit binaries
    */
    return (Elf64_Shdr *) (elf + header->e_shoff);
}

static Elf64_Sym *elf_section_symbols(char *elf, Elf64_Shdr *s_header)
{
    return (Elf64_Sym *) (elf + s_header->sh_offset);
}

static char *elf_string_section(char *elf, Elf64_Shdr *s_headers,
                                size_t index)
{
    Elf64_Shdr *str_header = s_headers + index;

    return elf + str_header->sh_offset;
}

static char *elf_symbol_name(char *elf, Elf64_Shdr *s_headers,
                             Elf64_Shdr *sym_header, Elf64_Sym *sym)
{
    return (elf_string_section(elf, s_headers, sym_header->sh_link)
            + sym->st_name);
}


/*
* If ELF is invalid, behavior is undefined (it may throw an ELFException
*                                           or... segfault)
*/
void *get_address(char *function)
{
    size_t size = elf_size();

    void *elf = open_elf(size);

    Elf64_Ehdr *header = elf_header(elf);

    Elf64_Shdr *s_headers = elf_sections_header(elf, header);

    void *addr = NULL;

    for (size_t i = 0; i < header->e_shnum; ++i)
    {
        Elf64_Shdr *sh = s_headers + i;

        if (sh->sh_type != SHT_SYMTAB && sh->sh_type != SHT_DYNSYM)
            continue;

        Elf64_Sym *symbols = elf_section_symbols(elf, sh);

        // cf. line 65 (same here with sh->sh_entsize)
        size_t syms_nb = sh->sh_size / sizeof(Elf64_Sym);

        for (size_t j = 0; j < syms_nb; ++j)
        {
            Elf64_Sym *sym = symbols + j;

            if (!strcmp(elf_symbol_name(elf, s_headers, sh, sym), function))
            {
                addr = (void *) sym->st_value;

                goto ret;
            }
        }
    }

ret:
    if (munmap(elf, size))
        throw(IOException);

    return addr;
}
