/* Custom exception system, based on Francesco Nidito's one
 * http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html
 *
 * Major feature added is env stack (that allows to nest try/catch statements)
 */

/* Copyright (C) 2009-2015 Francesco Nidito 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal
 * in the Software without restriction, including without limitation the rights
 * to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies
 * of the Software, and to permit persons to whom the Software is furnished to
 * do
 * so, subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included in
 * all
 * copies or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE
 * SOFTWARE. 
 */

#ifndef EXCEPTIONS_H
# define EXCEPTIONS_H

# include <setjmp.h>

enum my_exception
{
    None            = 0,
    Exception       = 1,
    TraceException  = 2,
    WaitException   = 3,
    ScanfException  = 4
};

struct my_env_list
{
    jmp_buf *env;

    struct my_env_list *next;
};

void new_env(jmp_buf *env);

void delete_env(void);

void throw_exception(enum my_exception ex);

# define try do { jmp_buf ex_buf__; new_env(&ex_buf__); \
                 switch( setjmp(ex_buf__) ) { case 0: while(1) {
# define catch(x) break; case x:
# define finally break; } default: {
# define etry break; } }; delete_env(); } while(0)
# define throw(x) throw_exception(x)

#endif /* EXCEPTIONS_H */
