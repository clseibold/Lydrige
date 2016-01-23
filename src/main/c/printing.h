#pragma once

#include "structures.h"
#include "reading.h"
#include "../../mpc/headers/mpc.h"

void dval_expr_print(dval* v, char open, char close);
void dval_print_str(dval* v);
void dval_print_char(dval* v);
void dval_print(dval* v);
void dval_println(dval* v);
