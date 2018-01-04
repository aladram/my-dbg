#ifndef MY_ELF_H
# define MY_ELF_H

# include <elf.h>

void *elf_section(Elf64_Ehdr *header, Elf64_Shdr *s_headers,
                  char *name, uint32_t sh_type);

void *get_dynamic_entry(char *base_addr,
                        Elf64_Dyn *dyn_section, Elf64_Sxword d_tag);

void *get_address(char *function);

#endif /* MY_ELF_H */
