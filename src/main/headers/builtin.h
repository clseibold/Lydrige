#ifndef BUILTIN_H_INCLUDE_GUARD
#define BUILTIN_H_INCLUDE_GUARD

#include <string.h>

#include "structure.h"
#include "../../hashmap/headers/bstrlib.h"
#include "../../linenoise/headers/linenoise.h"

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

bool print_elem(dval arg, bool removeQuotations);
dval *builtin_print(denv *e, dval *args, unsigned int argc);
dval *builtin_clear(denv *a, dval *args, unsigned int argc);

void denv_add_builtins(denv *e);

#endif