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

#ifndef STRUCTURE_H_INCLUDE_GUARD
#define STRUCTURE_H_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include "mpc.h"
#include "hashmap.h"

#define internal static
#define global static
#define bool int
#define true 1
#define false 0

typedef enum DVAL_TYPE {
    DVAL_ANY,
    DVAL_INT,
    DVAL_DOUBLE,
    DVAL_CHARACTER,
    DVAL_STRING,
    DVAL_ERROR,
    DVAL_INFO,
    DVAL_FUNC,
    DVAL_LIST,
    
    // TODO: Not sure how to handle qexprs better?
    DVAL_EXPR, // Uses elements
    DVAL_QEXPR, // Uses elements
    DVAL_IDENT // Uses str
} DVAL_TYPE;

typedef struct dval dval;
typedef struct denv denv;
typedef dval *(*dbuiltin)(denv*, dval*, unsigned int); // Dval Array and argc

struct dval { // TODO: Reorder to use least amount of memory!
    DVAL_TYPE type;
    int constant;
    union {
        int integer;
        double doub;
        char character;
        char *str;
        dbuiltin func;
        // TODO: Not sure how to handle qexprs better?
        dval *elements; // For qexpressions, expressions and other list-like elements
    };
    unsigned int count; // For qexpressions and other list-like elements and strings
};

dval *dval_int(long integer);
dval *dval_double(double doub);
dval *dval_character(char character);
dval *dval_string(char *str);
dval *dval_error(char *str, ...);
dval *dval_func(dbuiltin func, int constant);
dval *dval_list(dval *elements, unsigned int count);
void dval_del(dval *d);
dval *dval_copy(dval *d);

struct denv {
    denv *parent;
    Hashmap *map;
};

denv *denv_new(void);
void denv_del(denv *e);

int running;

mpc_parser_t *Line;
mpc_parser_t *Command;
mpc_parser_t *Statement;
mpc_parser_t *Expression;
mpc_parser_t *Value;
mpc_parser_t *Double;
mpc_parser_t *Integer;
mpc_parser_t *Character;
mpc_parser_t *String;
mpc_parser_t *Identifier;
mpc_parser_t *List;
mpc_parser_t *Qexpression;

#endif
