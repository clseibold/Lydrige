#pragma once

#include "structures.h"
#include "reading.h"
#include "../../mpc/headers/mpc.h"

#include <stdio.h>

#define COL_RED "\x1b[31m"
#define COL_GREEN "\x1b[32m"
#define COL_YELLOW "\x1b[33m"
#define COL_BLUE "\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN "\x1b[36m"
#define COL_RESET "\x1b[0m"

internal void dval_expr_print(dval* v, char* open, char* close);
internal void dval_print_str(dval* v);
internal void dval_print_char(dval* v);
void dval_print(dval* v);
void dval_println(dval* v);
