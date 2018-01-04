#include "gnu_hash_table.h"

#include <elf.h>
#include <stdint.h>
#include <string.h>

#include "memory.h"
#include "my_elf.h"

/*
* GNU-style hash map
*
* Specs taken from:
*   https://sourceware.org/ml/binutils/2006-10/msg00377.html
*   https://flapenguin.me/2017/05/10/elf-lookup-dt-gnu-hash/
*   https://blogs.oracle.com/ali/gnu-hash-elf-sections
*   https://sourceware.org/git/?p=glibc.git;a=blob;f=elf/dl-lookup.c
*     (do_lookup_x source code (in dl-lookup.c) from glibc)
*/

/*
* Hash function
*
* Author: Dan Bernstein
*/
static uint_fast32_t dl_new_hash(const char *s)
{
    uint_fast32_t h = 5381;

    for (unsigned char c = *s; c != '\0'; c = *++s)
        h = h * 33 + c;

    return h & 0xFFFFFFFF;
}

struct my_gnu_ht_config
{
    uint32_t nbuckets;

    uint32_t symndx;

    uint32_t maskwords;

    uint32_t shift2;
};

struct my_gnu_ht
{
    struct my_gnu_ht_config *conf;

    uint64_t *bloom;

    uint32_t *buckets;

    uint32_t *symhash;
};

static struct my_gnu_ht get_gnu_hash(Elf64_Dyn *dyn_section, void *base_addr)
{
    struct my_gnu_ht ht;

    ht.conf = get_dynamic_entry(base_addr, dyn_section, DT_GNU_HASH);

    ht.bloom = (uint64_t *) (ht.conf + 1);

    ht.conf = (void *) read_memory(ht.conf, sizeof(*ht.conf));

    ht.buckets = (uint32_t *) (ht.bloom + ht.conf->maskwords);

    ht.symhash = ht.buckets + ht.conf->nbuckets;

    ht.bloom = (void *) read_memory(ht.bloom,
                                    sizeof(*ht.bloom) * ht.conf->maskwords);

    ht.buckets = (void *) read_memory(ht.buckets,
                                      sizeof(*ht.buckets) * ht.conf->nbuckets);

    /*
    * Keep ht.symhash as a pointer to tracee's memory because we don't know
    * dynsymcount
    */
    /* ht.symhash = read_memory(symbols, sizeof(*symhash)
                                * (dynsymcount - symndx)); */

    return ht;
}

void *get_address_gnu_hash(char *function, Elf64_Dyn *dyn_section,
                           void *base_addr)
{
    uint32_t hash = dl_new_hash(function);

    Elf64_Sym *symtab = get_dynamic_entry(base_addr, dyn_section, DT_SYMTAB);

    char *strtab = get_dynamic_entry(base_addr, dyn_section, DT_STRTAB);

    struct my_gnu_ht ht = get_gnu_hash(dyn_section, base_addr);

    if (ht.conf->maskwords != 1 || ht.bloom[0] != 0)
    {
        uint64_t mask = ht.bloom[(hash / 64) & (ht.conf->maskwords - 1)];

        uint8_t bit1 = (uint8_t) (hash & 63);
        
        uint8_t bit2 = (uint8_t) ((hash >> ht.conf->shift2) & 63);

        if (!((mask >> bit1) & (mask >> bit2) & 1))
            return NULL;
    }

    uint32_t i = ht.buckets[hash % ht.conf->nbuckets];

    if (i < ht.conf->symndx)
        return NULL;

    for (;; ++i)
    {
        uint32_t h = *((uint32_t *)
                       read_memory(ht.symhash + i - ht.conf->symndx,
                                   sizeof(h)));

        Elf64_Sym *sym = (void *) read_memory(symtab + i, sizeof(*sym));

        if (hash == (h & (~1)))
            if (!strcmp(function, read_mem_string(strtab + sym->st_name)))
                return (void *) sym->st_value;

        if (h & 1)
            break;
    }

    return NULL;
}
