/* Lydrige - A simple interpreted programming language inspired by Lisp.
 * Copyright (c) 2016, Christian Seibold
 * Under MIT License
 *
 * You can find latest source code at:
 * https://github.com/krixano/Lydrige
 *
 * -----------------------------------------------------------------------
 * MIT License
 *
 * Copyright (c) 2016 Christian Seibold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * -----------------------------------------------------------------------
 */

#ifndef BUILTIN_H_INCLUDE_GUARD
#define BUILTIN_H_INCLUDE_GUARD

#include <string.h>

#include <structure.h>
#include <bstrlib.h>
#include <linenoise.h>

dval *denv_put(denv *e, char *k, dval *v, int constant);
dval *denv_get(denv *e, char *k);

dval *builtin_add(denv *e, dval *args, unsigned int argc);
dval *builtin_subtract(denv *e, dval *args, unsigned int argc);
dval *builtin_multiply(denv *e, dval *args, unsigned int argc);
dval *builtin_divide(denv *e, dval *args, unsigned int argc);
dval *builtin_mod(denv *e, dval *args, unsigned int argc);
dval *builtin_succ(denv *e, dval *args, unsigned int argc);

dval *builtin_list(denv *a, dval *args, unsigned int argc);
dval *builtin_len(denv *a, dval *args, unsigned int argc);
dval *builtin_get(denv *a, dval *args, unsigned int argc);
dval *builtin_set(denv *a, dval *args, unsigned int argc);
dval *builtin_first(denv *a, dval *args, unsigned int argc);
dval *builtin_last(denv *a, dval *args, unsigned int argc);
dval *builtin_head(denv *a, dval *args, unsigned int argc);
dval *builtin_tail(denv *a, dval *args, unsigned int argc);
dval *builtin_join(denv *a, dval *args, unsigned int argc);

dval *builtin_def(denv *a, dval *args, unsigned int argc);
dval *builtin_const(denv *a, dval *args, unsigned int argc);
dval *builtin_typeof(denv *a, dval *args, unsigned int argc);
char *get_type_string(DVAL_TYPE type);
dval *builtin_cast(denv *a, dval *args, unsigned int argc);

dval *builtin_if(denv *a, dval *args, unsigned int argc);

dval *builtin_and(denv *a, dval *args, unsigned int argc);
dval *builtin_or(denv *a, dval *args, unsigned int argc);

bool print_elem(dval arg, bool removeQuotations);
dval *builtin_print(denv *e, dval *args, unsigned int argc);
dval *builtin_read(denv *e, dval *args, unsigned int argc);
dval *builtin_clear(denv *a, dval *args, unsigned int argc);
dval *builtin_exit(denv *a, dval *args, unsigned int argc);

void denv_add_builtins(denv *e);

#endif
