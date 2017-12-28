#include "exceptions.h"

#include <err.h>
#include <setjmp.h>
#include <stdlib.h>

#include "memory_utils.h"

static struct my_env_list *list;

void new_env(jmp_buf *env)
{
    struct my_env_list *l = my_malloc(sizeof(*l));

    l->env = env;

    l->next = list;

    list = l;
}

/*
* No check if list is not NULL: it is the responsibility of the user
* to make sure a call to new_env precedes each call to delete_env
*/
void delete_env(void)
{
    struct my_env_list *l = list;

    list = list->next;

    free(l);
}

void throw_exception(enum my_exception ex)
{
    if (!list)
        errx(1, "Uncatched exception %d", ex);

    longjmp(*list->env, ex);
}
