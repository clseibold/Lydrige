#ifndef STRUCTURE_H_INCLUDE_GUARD
#define STRUCTURE_H_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include "../../mpc/headers/mpc.h"
#include "../../hashmap/headers/hashmap.h"

#define internal static
#define global static
#define bool int
#define true 1
#define false 0

typedef enum DVAL_TYPE {
	DVAL_ANY, DVAL_INT, DVAL_DOUBLE, DVAL_CHARACTER, DVAL_ERROR, DVAL_FUNC, DVAL_LIST
} DVAL_TYPE;

typedef struct dval dval;
typedef struct denv denv;
typedef dval *(*dbuiltin)(denv*, dval*, unsigned int); // Dval Array and argc

struct dval {
	DVAL_TYPE type;
	int constant;
	union {
		int integer;
		double doub;
		char character;
		char *str;
		dbuiltin func;
		dval *elements; // For qexpressions and other list-like elements
	};
	unsigned int count; // For qexpressions and other list-like elements
};

dval *dval_int(long integer);
dval *dval_double(double doub);
dval *dval_character(char character);
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

#endif