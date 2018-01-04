#ifndef LIBUNWIND_WRAPPER_H
# define LIBUNWIND_WRAPPER_H

void init_libunwind(void);

void print_backtrace(void);

void *get_finish_address(void);

void destroy_libunwind(void);

#endif /* LIBUNWIND_WRAPPER_H */
