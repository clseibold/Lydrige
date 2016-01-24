#pragma once

#include "../../mpc/headers/mpc.h"
#include "hashtable.h"

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
#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)
#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

struct dval;
struct denv;
union ddata;
typedef struct dval dval;
typedef struct denv denv;
typedef union ddata ddata;
typedef unsigned char byte;

enum { DDATA_INT, DDATA_DOUBLE, DDATA_BYTE, DDATA_STRING, DDATA_CHAR, DVAL_ERR, DVAL_SYM, DVAL_USYM, DVAL_SEXPR, DVAL_QEXPR, DVAL_FUNC };

typedef dval*(*dbuiltin)(denv*, dval*);

/*struct stack {
	dval** items = malloc(181818 * sizeof(dval)); // Stack can contain max of 181818 items at one time
	dval* add(dval* a) {

	}
} stack;*/

union ddata {
	long integer;
	double doub;
	byte b : 8; // Fix this, this should be an unsigned char (byte) with 8 bits!
	char character;
	char* str;
};

struct dval {
	int type;
	union ddata* content;
	int constant; // Boolean of whether value is constant. Only used if symbol is associated with value.

	/*char* err;
	char* sym;*/

	dbuiltin builtin;
	denv* env;
	dval* formals;
	dval* body;

	int count;
	struct dval** cell;
};

struct denv {
	denv* par;
	int count;
	/*char** syms;
	dval** vals; // TODO: Switch to hashtable!*/
	HASHTBL* hashtbl;
};

static int running = 1;

/* Parsers */
mpc_parser_t* Expr;
mpc_parser_t* Data;
mpc_parser_t* Double;
mpc_parser_t* Integer;
mpc_parser_t* Byte;
mpc_parser_t* Comment;
mpc_parser_t* String;
mpc_parser_t* Character;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Line;

char* dtype_name(int t);

/* Constructors */
denv* denv_new(void);
dval* dval_int(long x);
dval* dval_double(double x);
dval* dval_byte(byte x);
dval* dval_string(char* str);
dval* dval_char(char character);
dval* dval_err(char* fmt, ...);
dval* dval_sym(char* s);
dval* dval_usym(char* s);
dval* dval_sexpr(void);
dval* dval_qexpr(void);
dval* dval_func(dbuiltin func);
dval* dval_lambda(dval* formals, dval* body);

/* Destructors */
void dval_del(dval* v);
void denv_del(denv* e);

dval* dval_add(dval* v, dval* x);

/* Copying */
union ddata* ddata_copy(int type, union ddata* d);
dval* dval_copy(dval* v);
denv* denv_copy(denv* e);
