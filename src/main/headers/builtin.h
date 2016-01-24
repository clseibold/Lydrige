#pragma once

#include "structures.h"
#include "printing.h"
#include "reading.h"
#include <math.h>
#include "../../mpc/headers/mpc.h"

dval* dval_pop(dval* v, int i);
dval* dval_take(dval* v, int i);
dval* dval_join(dval* x, dval* y);
int dval_eq(dval* x, dval* y);
dval* denv_get(denv* e, dval* k);
void denv_put(denv* e, dval* k, dval* v, int constant);
void denv_def(denv* e, dval* k, dval* v, int constant);
void denv_const(denv* e, dval* k, dval* v); // TODO

dval* builtin_len(denv* e, dval* a);
dval* builtin_first(denv* e, dval* a);
dval* builtin_last(denv* e, dval* a);
dval* builtin_head(denv* e, dval* a);
dval* builtin_tail(denv* e, dval* a);
dval* builtin_list(denv* e, dval* a);
dval* builtin_eval(denv* e, dval* a);
dval* dval_call(denv* e, dval* f, dval* a);
dval* builtin_join(denv* e, dval* a);
dval* builtin_op(denv* e, dval* a, char* op);
dval* builtin_var(denv* e, dval* a, char* func);
dval* builtin_def(denv* e, dval* a);
dval* builtin_put(denv* e, dval* a);
dval* builtin_lambda(denv* e, dval* a);
dval* builtin_add(denv* e, dval* a);
dval* builtin_sub(denv* e, dval* a);
dval* builtin_mul(denv* e, dval* a);
dval* builtin_div(denv* e, dval* a);
dval* builtin_mod(denv* e, dval* a);
dval* builtin_ord(denv* e, dval* a, char* op);
dval* builtin_gt(denv* e, dval* a);
dval* builtin_lt(denv* e, dval* a);
dval* builtin_ge(denv* e, dval* a);
dval* builtin_le(denv* e, dval* a);
dval* builtin_cmp(denv* e, dval* a, char* op);
dval* builtin_eq(denv* e, dval* a);
dval* builtin_ne(denv* e, dval* a);
dval* builtin_not(denv* e, dval* a);
dval* builtin_and(denv* e, dval* a);
dval* builtin_or(denv* e, dval* a);
dval* builtin_min(denv* e, dval* a); // TODO
dval* builtin_if(denv* e, dval* a);
dval* builtin_load(denv* e, dval* a);
dval* builtin_concatenate(denv* e, dval* a); // TODO
dval* builtin_print(denv* e, dval* a);
dval* builtin_error(denv* e, dval* a);
dval* builtin_exit(denv* e, dval* a);

dval* dval_eval_sexpr(denv* e, dval* v);
dval* dval_eval(denv* e, dval* v);

void denv_add_builtin(denv* e, char* name, dbuiltin func);
void denv_add_builtins(denv* e);
