#pragma once

#include "../../mpc/headers/mpc.h"
#include <hashmap.h>

#define LASSERT(args, cond, fmt, ...) \
	if (!(cond)) { \
		dval* err = dval_err(fmt, ##__VA_ARGS__); \
		dval_del(args);\
		return err; \
	}
#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. " \
    "Got %s, Expected %s.", \
    func, index, dtype_name(args->cell[index]->type), dtype_name(expect))
#define LASSERT_MTYPE(func, args, index, cond, fmt, ...) \
	LASSERT(args, cond, \
	"Function '%s' passed incorrect type for argument %i. "\
	"Got %s, Expected " \
	fmt, func, index, dtype_name(args->cell[index]->type), ##__VA_ARGS__)
#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)
#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} or [] for argument %i.", func, index);

int running;
struct dval;
struct denv;
typedef struct dval dval;
typedef struct denv denv;
typedef unsigned char byte;

enum { DDATA_RANGE, DDATA_INT, DDATA_DOUBLE, DDATA_BYTE, DDATA_STRING, DDATA_CHAR, DVAL_ERR, DVAL_SYM, DVAL_TYPE, DVAL_SSEXPR, DVAL_SEXPR, DVAL_SQEXPR, DVAL_QEXPR, DVAL_SLIST, DVAL_LIST, DVAL_FUNC, DVAL_NOTE };

typedef dval*(*dbuiltin)(denv*, dval*);

struct dval {
	int type;
	union {
		long integer;
		double doub;
		byte b : 8;
		char character;
		char* str;
		int ttype; // TODO: Merge this with sym_type, they should be the same var!
	};
	long max; // Used for ranges
	int constant; // Boolean of whether value is constant. Only used if symbol is associated with value.
	int sym_type;

	dbuiltin builtin;
	denv* env;
	dval* formals;
	dval* body;

	unsigned int count;
	struct dval** cell;
};

struct denv {
	denv* par;
	int count;
	Hashmap *map;
};

/* Parsers */
mpc_parser_t* Expr;
mpc_parser_t* Data;
mpc_parser_t* Double;
mpc_parser_t* Range;
mpc_parser_t* Integer;
mpc_parser_t* Byte;
mpc_parser_t* Comment;
mpc_parser_t* String;
mpc_parser_t* Character;
mpc_parser_t* Symbol;
mpc_parser_t* Note;
mpc_parser_t* List;
mpc_parser_t* SList;
mpc_parser_t* SSexpr;
mpc_parser_t* Sexpr;
mpc_parser_t* SQexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Line;

char* dtype_name(int t);

/* Constructors */
denv* denv_new(void);
dval* dval_range(long min, long max);
dval* dval_int(long x);
dval* dval_double(double x);
dval* dval_byte(byte x);
dval* dval_string(char* str);
dval* dval_char(char character);
dval* dval_err(char* fmt, ...);
dval* dval_sym(char* s, int type);
dval* dval_type(int type);
dval* dval_sexpr(void);
dval* dval_ssexpr(void);
dval* dval_qexpr(void);
dval* dval_sqexpr(void);
dval* dval_list(void);
dval* dval_slist(void);
dval* dval_func(dbuiltin func);
dval* dval_lambda(dval* formals, dval* body);
dval* dval_note(void);

/* Destructors */
void denv_del(denv* e);
void dval_del(dval* v);

dval* dval_add(dval* v, dval* x);

/* Copying */
dval* ddata_copy(int type, dval* d, dval* x);
dval* dval_copy(dval* v);
denv* denv_copy(denv* e);
