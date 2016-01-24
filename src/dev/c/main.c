#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../mpc/headers/mpc.h"

#ifdef _WIN32
static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <readline/readline.h>
#include <readline/history.h>
#endif

#define LASSERT(args, cond, err) \
	if (!(cond)) { lval_del(args); return lval_err(err); }


typedef struct lval {
    int type;
    long num;

	char* err;
	char* sym;

	int count;
	struct lval **cell;
} lval;

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/** Returns a pointer to an lval with type LVAL_NUM */
lval *lval_num(long x) {
    lval *v = (lval*) malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval *lval_sym(char* s) {
	lval *v = (lval*) malloc(sizeof(lval));
	v->type = LVAL_SYM;
	v->sym = (char*) malloc(strlen(s) + 1);
	strcpy(v->sym, s);
	return v;
}

lval *lval_sexpr() {
	lval *v = (lval*) malloc(sizeof(lval));
	v->type = LVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

lval *lval_qexpr() {
	lval *v = (lval*) malloc(sizeof(lval));
	v->type = LVAL_QEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

/** Returns a pointer to an lval with type LVAL_ERR */
lval *lval_err(char *m) {
    lval *v = (lval*) malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = (char*) malloc(strlen(m) + 1);
	strcpy(v->err, m);
    return v;
}

// Free the lval and its children from the heap
void lval_del(lval *v) {
	switch(v->type) {
		case LVAL_NUM: break;
		case LVAL_ERR: free(v->err); break;
		case LVAL_SYM: free(v->sym); break;
		case LVAL_QEXPR:
		case LVAL_SEXPR:
			for (int i = 0; i < v->count; i++) {
				lval_del(v->cell[i]);
			}
			free(v->cell);
			break;
	}

	free(v);
}

// Add to lval v a child lval x. Reallocates cell
lval *lval_add(lval *v, lval *x) {
	v->count++;
	v->cell = (lval**) realloc(v->cell, sizeof(lval*) * v->count);
	v->cell[v->count-1] = x;
	return v;
}

void lval_print(lval *v);

/** Prints the expression as a whole, including any children.
	Does not evaluate anything, yet
	*/
void lval_expr_print(lval *v, char open, char close) {
    putchar(open);
	for (int i = 0; i < v->count; i++) {
		lval_print(v->cell[i]);

		// Print trailing space unless last item
		if (i != (v->count - 1)) putchar(' ');
	}
	putchar(close);
}

void lval_print(lval *v) {
	switch(v->type) {
		case LVAL_NUM:		printf("%li", v->num); break;
		case LVAL_ERR:		printf("ERROR: %s", v->err); break;
		case LVAL_SYM:		printf("%s", v->sym); break;
		case LVAL_SEXPR:	lval_expr_print(v, '(', ')'); break;
		case LVAL_QEXPR:	lval_expr_print(v, '{', '}'); break;
	}
}

void lval_println(lval *v) {
	lval_print(v);
	putchar('\n');
}

// Pops lval from list and returns it. Does NOT delete the list.
lval *lval_pop(lval *v, int i) {
	// Find the item at "i"
	lval *x = v->cell[i];

	// Shift memory after the item at i over the top
	memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));

	// Decrease the count of items in the list
	v->count--;

	// Reallocate the memory unused
	v->cell = (lval**) realloc(v->cell, sizeof(lval*) * v->count);
	return x;
}

// Pops lval from list and returns it. Deletes the list!
lval *lval_take(lval *v, int i) {
	lval *x = lval_pop(v, i);
	lval_del(v);
	return x;
}

lval *builtin(lval *a, char *func);
lval *builtin_op(lval *a, char *op);

lval *lval_eval(lval *v);

lval *lval_eval_sexpr(lval *v) {
	// Evaluate children
	for (int i = 0; i < v->count; i++) {
		v->cell[i] = lval_eval(v->cell[i]);
		if (v->cell[i]->type == LVAL_ERR) return lval_take(v, i);
	}

	// Expty expression
	if (v->count == 0) return v;

	// Single expression
	if (v->count == 1) return lval_take(v, 0);

	// Ensure first element is a symbol
	lval *f = lval_pop(v, 0);
	if (f->type != LVAL_SYM) {
		lval_del(f);
		lval_del(v);
		return lval_err((char*)"S-expression Does not start with symbol!");
	}

	lval *result = builtin(v, f->sym);
	lval_del(f);
	return result;
}

lval *lval_eval(lval *v) {
	// Evaluate sexpressions
	if (v->type == LVAL_SEXPR) return lval_eval_sexpr(v);
	return v;
}

lval *lval_join(lval *x, lval *y) {
	// For each cell in 'y' add it to 'x'
	while (y->count) {
		x = lval_add(x, lval_pop(y, 0));
	}

	// Delete the empty 'y' and return 'x'
	lval_del(y);
	return x;
}

lval *builtin_op(lval *a, char *op) {
	// Ensure all arguments are numbers
	for (int i = 0; i < a->count; i++) {
		LASSERT(a, a->cell[i]->type == LVAL_NUM, (char*) "Cannot operate on non-number argument!");
		/*if (a->cell[i]->type != LVAL_NUM) {
			lval_del(a);
			return lval_err((char*)"Cannot operate on non-number argument!");
		}*/
	}

	// Pop the first element
	lval *x = lval_pop(a, 0);

	// If no arguments and sub then perform unary negation
	if (!strcmp(op, "-") && a->count == 0) {
		x->num = -x->num;
		lval_del(a);
		return x;
	}

	// While there are still elements remaining
	while (a->count > 0) {
		// Pop the next element
		lval *y = lval_pop(a, 0);

		if (!strcmp(op, "+")) x->num += y->num;
		else if (!strcmp(op, "-")) x->num -= y->num;
		else if (!strcmp(op, "*")) x->num *= y->num;
		else if (!strcmp(op, "/")) {
			if (y->num == 0) {
				lval_del(x);
				lval_del(y);
				return lval_err((char*)"Division By Zero!");
			}
			x->num /= y->num;
		} else if (!strcmp(op, "%")) {
			if (y->num == 0) {
				lval_del(x);
				lval_del(y);
				return lval_err((char*)"Division By Zero!");
			}
			x->num %= y->num;
		} else if (!strcmp(op, "^")) x->num = pow(x->num, y->num);
		lval_del(y);
	}

	lval_del(a);
	return x;
}

lval *builtin_head(lval *a) {
	// Check error conditions
	LASSERT(a, a->count == 1, (char*) "Function 'head' was passed too many arguments!");
	LASSERT(a, a->cell[0]->type == LVAL_QEXPR, (char*) "Function 'head' passed incorrect type!");
	LASSERT(a, a->cell[0]->count != 0, (char*) "Function 'head' passed {}! A filled q-expression is required.")

	// Take first argument
	lval *v = lval_take(a, 0);

	// Delete all elements that are not head and return
	while (v->count > 1) {
		lval_del(lval_pop(v, 1));
	}
	return v;
}

lval *builtin_tail(lval *a) {
	// Check error conditions
	LASSERT(a, a->count == 1, (char*) "Function 'tail' was passed too many arguments!");
	LASSERT(a, a->cell[0]->type == LVAL_QEXPR, (char*) "Function 'tail' passed incorrect type!");
	LASSERT(a, a->cell[0]->count != 0, (char*) "Function 'tail' passed {}! A filled q-expression is required.")

	// Take the first argument
	lval *v = lval_take(a, 0);

	// Delete first element and return
	lval_del(lval_pop(v, 0));
	return v;
}

lval *builtin_list(lval *a) {
	a->type = LVAL_QEXPR;
	return a;
}

lval *builtin_eval(lval *a) {
	LASSERT(a, a->count == 1, (char*) "Function 'eval' was passed too many arguments!");
	LASSERT(a, a->cell[0]->type == LVAL_QEXPR, (char*) "Function 'eval' passed incorrect type!");

	lval *x = lval_take(a, 0);
	x->type = LVAL_SEXPR;
	return lval_eval(x);
}

lval *builtin_join(lval *a) {
	for (int i = 0; i < a->count; i++) {
		LASSERT(a, a->cell[i]->type == LVAL_QEXPR, (char*) "Function 'join' passed incorrect type.");
	}

	lval *x = lval_pop(a, 0);

	while (a->count) {
		x = lval_join(x, lval_pop(a, 0));
	}

	lval_del(a);
	return x;
}

lval *builtin(lval *a, char *func) {
	if (!strcmp("list", func)) return builtin_list(a);
	else if (!strcmp("head", func)) return builtin_head(a);
	else if (!strcmp("tail", func)) return builtin_tail(a);
	else if (!strcmp("join", func)) return builtin_join(a);
	else if (!strcmp("eval", func)) return builtin_eval(a);
	else if (strstr("+-/*%^", func)) return builtin_op(a, func);
	lval_del(a);
	return lval_err((char*)"Unknown Function!");
}

lval *lval_read_num(mpc_ast_t *t) {
	errno = 0;
	long x = strtol(t->contents, NULL, 10);
	return errno != ERANGE ? lval_num(x) : lval_err((char*)"Invalid number!");
}

lval *lval_read(mpc_ast_t *t) {
	if (strstr(t->tag, "number")) return lval_read_num(t);
	if (strstr(t->tag, "symbol")) return lval_sym(t->contents);

	lval *x = NULL;
	if (strcmp(t->tag, ">") == 0) x = lval_sexpr();
	if (strstr(t->tag, "sexpr")) x = lval_sexpr();
	if (strstr(t->tag, "qexpr")) x = lval_qexpr();

	// For each of the children, if an expression, add to children of lval
	for (int i = 0; i < t->children_num; i++) {
		if (!strcmp(t->children[i]->contents, "(")) continue;
		if (!strcmp(t->children[i]->contents, ")")) continue;
		if (!strcmp(t->children[i]->contents, "{")) continue;
		if (!strcmp(t->children[i]->contents, "}")) continue;
		if (!strcmp(t->children[i]->tag, "regex")) continue;
		// Adds the lval as a child to lval x
		// Reads in the lval before (so all of its children are read also)
		x = lval_add(x, lval_read(t->children[i]));
	}

	return x;
}

int main(int argc, char** argv) {
    mpc_parser_t *Number = mpc_new("number");
	mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
	mpc_parser_t *Qexpr = mpc_new("qexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Line = mpc_new("line");

    mpca_lang(MPCA_LANG_DEFAULT,
        " \
            number      : /-?[0-9]+/ ; \
			symbol		: \"head\" | \"list\" | \"tail\" | \"join\" | \"eval\" | '+' | '-' | '*' | '/' | '%' | '^' ; \
			sexpr		: '(' <expr>* ')' ; \
			qexpr		: '{' <expr>* '}' ; \
            expr        : <number> | <symbol> | <sexpr> | <qexpr> ; \
            line        : /^/ <expr>* /$/ ; \
        ", Number, Symbol, Sexpr, Qexpr, Expr, Line);

    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char *input = readline("DLisp -> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Line, &r)) {
			mpc_ast_print((mpc_ast_t*)r.output);
            //lval result = eval((mpc_ast_t*)r.output);
			lval *x = lval_eval(lval_read((mpc_ast_t*)r.output));
			lval_println(x);
			lval_del(x);

            mpc_ast_delete((mpc_ast_t*)r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Line);
    return 0;
}
