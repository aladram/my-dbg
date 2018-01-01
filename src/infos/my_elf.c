#include "my_elf.h"

#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "exceptions.h"
#include "file_utils.h"
#include "format_utils.h"
#include "memory.h"
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

void *get_address_exec(char *function, void *elf,
                       Elf64_Ehdr *header, Elf64_Shdr *s_headers)
{
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

            if (ELF64_ST_TYPE(sym->st_info) != STT_FUNC)
                continue;

            if (!strcmp(elf_symbol_name(elf, s_headers, sh, sym), function))
                return (void *) sym->st_value;
        }
    }

    return NULL;
}

static Elf64_auxv_t *get_auxiliary_vector()
{
    FILE *f = fopen(get_proc_path("auxv"), "r");

    if (!f)
        throw(IOException);

    Elf64_auxv_t *auxv = NULL;

    for (size_t length = 1;; ++length)
    {
        auxv = my_realloc(auxv, sizeof(*auxv) * length);

        if (fread(auxv + length - 1, sizeof(*auxv), 1, f) != 1)
            throw(IOException);

        if (auxv[length - 1].a_type == AT_NULL)
            break;
    }

    if (fclose(f) == EOF)
        throw(IOException);

    return auxv;
}


static Elf64_Phdr *extract_program_headers(Elf64_auxv_t *auxv)
{
    for (; auxv->a_type != AT_NULL; ++auxv)
        if (auxv->a_type == AT_PHDR)
            return (Elf64_Phdr *) auxv->a_un.a_val;

    throw(ELFException);

    return NULL;
}

static Elf64_Phdr *get_program_header(Elf64_Ehdr *header, Elf64_Phdr *p_headers,
                                      Elf64_Word p_type)
{
    for (size_t i = 0; i < header->e_phnum; ++i)
    {
        Elf64_Phdr *ph = (void *) read_memory(p_headers + i, sizeof(*ph));

        if (ph->p_type == p_type)
            return ph;
    }

    throw(ELFException);

    return NULL;
}

void *get_base_address(Elf64_Ehdr *header, Elf64_Phdr *p_headers)
{
    Elf64_Phdr *phdr_ph = get_program_header(header, p_headers, PT_PHDR);

    return (char *) p_headers - phdr_ph->p_vaddr;
}

Elf64_Dyn *get_dynamic_section(Elf64_Ehdr *header, Elf64_Phdr *p_headers,
                               void *base_addr)
{
    Elf64_Phdr *dyn_ph = get_program_header(header, p_headers, PT_DYNAMIC);

    return (Elf64_Dyn *) ((char *) base_addr + dyn_ph->p_vaddr);
}

void *get_dynamic_entry(char *base_addr,
                        Elf64_Dyn *dyn_section, Elf64_Sxword d_tag)
{
    for (;; ++dyn_section)
    {
        Elf64_Dyn *dyn = (void *) read_memory(dyn_section, sizeof(*dyn));

        if (dyn->d_tag == d_tag)
            return base_addr + dyn->d_un.d_ptr;

        if (dyn->d_tag == DT_NULL)
            break;
    }

    throw(ELFException);

    return NULL;
}

/*
* If ELF is invalid, behavior is undefined (it may throw an ELFException
*                                           or... segfault)
*/
void *get_address(char *function)
{
    size_t size = elf_size();

    void *elf = open_elf(size);

    void *addr = NULL;

    Elf64_Ehdr *header = elf_header(elf);

    Elf64_Shdr *s_headers = elf_sections_header(elf, header);

    if (header->e_type == ET_EXEC)
        addr = get_address_exec(function, elf, header, s_headers);

    else if (header->e_type == ET_DYN)
    {
        Elf64_auxv_t *auxv = get_auxiliary_vector();

        Elf64_Phdr *p_headers = extract_program_headers(auxv);

        void *base_addr = get_base_address(header, p_headers);

        void *tmp_addr = get_address_exec(function, elf, header, s_headers);

        if (tmp_addr)
            return (char *) base_addr + (size_t) tmp_addr;

        Elf64_Dyn *dyn_section = get_dynamic_section(header, p_headers, base_addr);

        Elf64_Sym *symtab = get_dynamic_entry(base_addr, dyn_section, DT_SYMTAB);

        char *strtab = get_dynamic_entry(base_addr, dyn_section, DT_STRTAB);

        size_t size = (strtab - (char *) symtab) / sizeof(Elf64_Sym);

        for (size_t i = 0; i < size; ++i)
        {
            Elf64_Sym *sym = (void *) read_memory(symtab + i, sizeof(*sym));

            //HARDCODE
            char *str = read_memory(strtab + sym->st_name, 16);

            if (ELF64_ST_TYPE(sym->st_info) != STT_FUNC)
                continue;

            printf("Debug: '%s' -> %p\n", str, (void *) sym->st_value);

            if (!strcmp(str, function))
            {
                addr = (void *) sym->st_value;

                break;
            }
        }
    }

    else
        throw(ELFException);

    if (munmap(elf, size))
        throw(IOException);

    return addr;
}
