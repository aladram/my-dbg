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

#define CASE_EX_NAME(Ex) case Ex: \
                             return #Ex;

static char *exception_name(enum my_exception ex)
{
    switch (ex)
    {
    CASE_EX_NAME(None);
    CASE_EX_NAME(Exception);
    CASE_EX_NAME(PtraceException);
    CASE_EX_NAME(WaitException);
    CASE_EX_NAME(ScanfException);
    CASE_EX_NAME(PrintfException);
    CASE_EX_NAME(IOException);
    CASE_EX_NAME(MemoryException);
    CASE_EX_NAME(ELFException);
    CASE_EX_NAME(SyscallException);
    CASE_EX_NAME(DisasmException);
    }

    return "UnknownException";
}

void throw_exception(enum my_exception ex)
{
    if (!list)
        errx(1, "Uncatched exception %s", exception_name(ex));

    longjmp(*list->env, ex);
}

size_t ex_depth(void)
{
    size_t depth = 0;

    struct my_env_list *l = list;

    for (; l; l = l->next)
        ++depth;

    return depth;
}
