#include "temp_memory_utils.h"

#include <stdlib.h>
#include <string.h>

#include "exceptions.h"
#include "memory_utils.h"

struct my_ptr_list
{
    void *ptr;

    struct my_ptr_list *next;
};

static struct my_ptr_list *list;

static void add_ptr(void *ptr)
{
    struct my_ptr_list *l = my_malloc(sizeof(*l));

    l->ptr = ptr;

    l->next = list;

    list = l;
}

static void tmp_free_rec(struct my_ptr_list *l)
{
    if (!l)
        return;

    struct my_ptr_list *next = l->next;

    free(l->ptr);

    free(l);

    tmp_free_rec(next);
}

void tmp_free_all(void)
{
    tmp_free_rec(list);

    list = NULL;
}

void *tmp_malloc(size_t size)
{
    void *ptr = malloc(size);

    if (!ptr)
        throw(MemoryException);

    add_ptr(ptr);

    return ptr;
}

void *tmp_realloc(void *old_ptr, size_t old_size, size_t new_size)
{
    if (list && list->ptr == old_ptr)
    {
        list->ptr = realloc(old_ptr, new_size);

        if (!list->ptr)
            throw(MemoryException);

        return list->ptr;
    }

    void *ptr = malloc(new_size);

    if (!ptr)
        throw(MemoryException);

    add_ptr(ptr);

    if (old_ptr)
        memcpy(ptr, old_ptr, old_size < new_size ? old_size : new_size);

    return ptr;
}

void *tmp_calloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);

    if (!ptr)
        throw(MemoryException);

    add_ptr(ptr);

    return ptr;
}

char *tmp_strdup(const char *s)
{
    char *s2 = tmp_malloc(strlen(s) + 1);

    strcpy(s2, s);

    return s2;
}
