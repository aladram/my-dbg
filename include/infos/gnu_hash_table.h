#ifndef GNU_HASH_TABLE_H
# define GNU_HASH_TABLE_H

# include <elf.h>

void *get_address_gnu_hash(char *function, Elf64_Dyn *dyn_section,
                           void *base_addr);

#endif /* GNU_HASH_TABLE_H */
