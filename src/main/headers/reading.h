#ifndef READING_H_INCLUDE_GUARD
#define READING_H_INCLUDE_GUARD

#include "structures.h"
#include "../../mpc/headers/mpc.h"

dval* dval_read_lambda(denv* e, mpc_ast_t* t);
dval* dval_read_range(mpc_ast_t* t);
dval* dval_read_int(mpc_ast_t* t);
dval* dval_read_double(mpc_ast_t* t);
dval* dval_read_byte(mpc_ast_t* t);
dval* dval_read_string(mpc_ast_t* t);
dval* dval_read_character(mpc_ast_t* t);
dval* dval_read(denv* e, mpc_ast_t* t);

#endif
