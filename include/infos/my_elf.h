#ifndef MY_ELF_H
# define MY_ELF_H

# include <elf.h>
# include <stddef.h>

struct my_elf
{
    size_t size;

    int fd;

    void *elf;

    Elf64_Ehdr *header;

    Elf64_Shdr *s_headers;
};

struct my_elf_section
{
    Elf64_Shdr *sh;

    void *addr;
};

struct my_elf *open_elf(void);

struct my_elf_section *elf_section(struct my_elf *elf, char *name,
                                   uint32_t sh_type);

void *get_dynamic_entry(char *base_addr,
                        Elf64_Dyn *dyn_section, Elf64_Sxword d_tag);

void *get_address(char *function);

void close_elf(struct my_elf *my_elf);

#endif /* MY_ELF_H */
