#include "../headers/builtin.h"
#include <bstrlib.h>

/* Returns extracted element at index i, shifts list elements backwards to fill in where element was at */
dval* dval_pop(dval* v, int i) {
	dval* x = v->cell[i];
	memmove(&v->cell[i], &v->cell[i + 1],
		sizeof(dval*) * (v->count - i - 1));

	v->count--;

	v->cell = (dval**)realloc(v->cell, sizeof(dval*) * v->count);
	return x;
}

/* Returns extracted element at index i, deletes the list */
dval* dval_take(dval* v, int i) {
	dval* x = dval_pop(v, i);
	dval_del(v);
	return x;
}

/* Joins two dvals */
dval* dval_join(dval* x, dval* y) {
	while (y->count) {
		x = dval_add(x, dval_pop(y, 0));
	}

	dval_del(y);
	return x;
}

/** Determines whether two dvals are equal.
  * If they are not, a zero is returned.
  */
int dval_eq(dval* x, dval* y) {
	if (x->type != y->type) {
		return 0;
	}

	switch (x->type) {
	case DDATA_RANGE:
		return (x->content->integer == y->content->integer && x->max == y->max);
	case DDATA_INT:
		return (x->content->integer == y->content->integer);
	case DDATA_DOUBLE:
		return (x->content->doub == y->content->doub);
	case DDATA_BYTE:
		return (x->content->b == y->content->b);
	case DDATA_CHAR:
		return (x->content->character == y->content->character);
	case DDATA_STRING:
		return (strcmp(x->content->str, y->content->str) == 0);
	case DVAL_ERR:
		return (strcmp(x->content->str, y->content->str) == 0);
	case DVAL_TYPE:
		return (x->content->type == y->content->type);
	case DVAL_SYM:
		return (strcmp(x->content->str, y->content->str) == 0);
	case DVAL_FUNC:
		if (x->builtin || y->builtin) {
			return x->builtin == y->builtin;
		}
		else {
			return dval_eq(x->formals, y->formals) && dval_eq(x->body, y->body);
		}
	case DVAL_LIST:
	case DVAL_QEXPR:
	case DVAL_SEXPR:
		if (x->count != y->count) {
			return 0;
		}
		for (unsigned int i = 0; i < x->count; i++) {
			if (!dval_eq(x->cell[i], y->cell[i])) {
				return 0;
			}
		}
		return 1;
	}
	return 0;
}

dval* denv_get(denv* e, dval* k) {
	dval* d;
	if ((d = (dval*)Hashmap_get(e->map, bfromcstr(k->content->str)))) {
		return dval_copy(d);
	}

	if (e->par) {
		return denv_get(e->par, k);
	}
	// TODO: Return unbound symbol type!
	return dval_err((char*)"Unbound symbol '%s'", k->content->str);
	// dval_usym(k->sym);
}

void denv_put(denv* e, dval* k, dval* v, int constant) { // TODO: does this work correctly?
	dval* t;
	dval* item = (dval*)Hashmap_get(e->map, bfromcstr(k->content->str));
	if (item != NULL) { // If already defined in hashtable
		if (item->constant == 0) { // If not constant (in hashtable)
			dval* deleted = (dval*)Hashmap_delete(e->map, bfromcstr(k->content->str));
			dval_del(deleted); // Note that `deleted` is the same as `item`!
			item = NULL;
			t = dval_copy(v); // Copy value into t
			t->constant = constant; // set constant
			Hashmap_set(e->map, bfromcstr(k->content->str), t); // TODO: Check for errors!
			return;
		} else {
			printf("Error: Cannot edit '%s'. It is a constant\n", k->content->str);
			return;
		}
	} else { // Not in hashtable yet!
		e->count++;
		t = dval_copy(v); // Copy value into t
		t->constant = constant; // set constant
		Hashmap_set(e->map, bfromcstr(k->content->str), t);
		return;
	}
}

void denv_def(denv* e, dval* k, dval* v, int constant) {
	while (e->par) { // Find root environment
		e = e->par;
	}
	denv_put(e, k, v, constant);
}

/* Returns length of given Q-Expression */
dval* builtin_len(denv* e, dval* a) {
	LASSERT_NUM("len", a, 1);
	LASSERT_MTYPE("len", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));

	dval* x = dval_int(a->cell[0]->count);
	dval_del(a);
	return x;
}

/** Returns the item at given index from given q-expression or list.
  * The q-expression or list is deleted.
  */
dval* builtin_get(denv* e, dval* a) { // Make work for strings
	LASSERT_NUM("get", a, 2);
	LASSERT_TYPE("get", a, 0, DDATA_INT);
	LASSERT_MTYPE("get", a, 1, a->cell[1]->type == DVAL_QEXPR || a->cell[1]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT(a, a->cell[0]->content->integer >= 0,
		"Index must be zero or positive.");
	LASSERT(a, a->cell[0]->content->integer < a->cell[1]->count && a->cell[0]->content->integer >= 0,
		"Index out of bounds. Max index allowed: %i", a->cell[1]->count - 1);

	dval* result = dval_eval(e, dval_pop(a->cell[1], a->cell[0]->content->integer));
	dval_del(a);
	return result;
}

/** Very similar to the `get` function, except, what is returned is not automatically semi-evaluated.
 */
dval* builtin_extract(denv* e, dval* a) {
	LASSERT_NUM("get", a, 2);
	LASSERT_TYPE("get", a, 0, DDATA_INT);
	LASSERT_MTYPE("get", a, 1, a->cell[1]->type == DVAL_QEXPR || a->cell[1]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT(a, a->cell[0]->content->integer >= 0,
		"Index must be zero or positive.");
	LASSERT(a, a->cell[0]->content->integer < a->cell[1]->count && a->cell[0]->content->integer >= 0,
		"Index out of bounds. Max index allowed: %i", a->cell[1]->count - 1);

	dval* result = dval_pop(a->cell[1], a->cell[0]->content->integer);
	dval_del(a);
	return result;
}

/** Sets the item at given index from given q-expression or list to a given value and returns
  *   the new list as a q-expression. Ex: `set index qexpr/list value`
  */
dval* builtin_set(denv* e, dval* a) {
	LASSERT_NUM("set", a, 3);
	LASSERT_TYPE("set", a, 0, DDATA_INT);
	LASSERT_MTYPE("set", a, 1, a->cell[1]->type == DVAL_QEXPR || a->cell[1]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT(a, a->cell[0]->content->integer >= 0,
		"Index must be zero or positive.");
	LASSERT(a, a->cell[0]->content->integer < a->cell[1]->count && a->cell[0]->content->integer >= 0,
		"Index out of bounds. Max index allowed: %i", a->cell[1]->count - 1);

	dval* result = dval_pop(a, 1);
	result->cell[a->cell[0]->content->integer] = dval_pop(a, 1);
	dval_del(a);
	return result;
}

dval* builtin_typeof(denv* e, dval* a) {
	LASSERT_NUM("typeof", a, 1);

	dval* result = dval_type(a->cell[0]->type);
	dval_del(a);
	return result;
}

dval* builtin_throw(denv* e, dval* a) {
	dval_del(a);
	return dval_sexpr();
}

dval* builtin_to_list(denv* e, dval* a) {  // TODO: List in README.md
	LASSERT_NUM("to_list", a, 1);
	LASSERT_TYPE("to_list", a, 0, DVAL_QEXPR);

	dval* result = dval_take(a, 0);
	result->type = DVAL_LIST;
	return dval_eval(e, result); // Should result be semi-evaluated
}

dval* builtin_to_qexpr(denv* e, dval* a) {
	LASSERT_NUM("to_qexpr", a, 1);
	LASSERT_TYPE("to_qexpr", a, 0, DVAL_LIST);

	dval* result = dval_take(a, 0);
	result->type = DVAL_QEXPR;
	return dval_eval(e, result);
}

dval* builtin_while(denv* e, dval* a) { // TODO: cleanup 
	LASSERT_NUM("while", a, 2);
	LASSERT_MTYPE("while", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT_MTYPE("while", a, 1, a->cell[1]->type == DVAL_QEXPR || a->cell[1]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));

	dval* conditional = dval_pop(a, 0);
	conditional->type = DVAL_SEXPR;
	dval* cond = dval_eval(e, dval_copy(conditional));
	//dval* conditional = builtin_eval(e, dval_copy(a->cell[0]));
	dval* body = dval_pop(a, 0);
	body->type = DVAL_SEXPR;
	if (cond->type != DDATA_INT) {
		dval_del(conditional);
		dval_del(cond);
		dval_del(body);
		dval_del(a);
		return dval_err("Argument 1 must evaluate to a conditional/integer.");
	}
	while (cond->content->integer) { // Not stopping correctly
		dval* eval = dval_eval(e, dval_copy(body));
		if (eval->type == DVAL_ERR) {
			dval_del(conditional);
			dval_del(cond);
			dval_del(body);
			dval_del(a);
			return eval;
		}
		// TODO: Add the returns to a q-expression that will be returned at the end of the while loop.
		dval_del(eval);
		dval_del(cond);
		cond = dval_eval(e, dval_copy(conditional));
	}

	dval_del(conditional);
	dval_del(cond);
	dval_del(body);
	dval_del(a);
	return dval_qexpr();
}

/* Returns Q-Expression of first element given a Q-Expression or List Literal */
dval* builtin_first(denv* e, dval* a) {
	LASSERT_NUM("first", a, 1);
	LASSERT_MTYPE("first", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT_NOT_EMPTY("first", a, 0);

	// Take first argument
	dval* v = dval_take(a, 0);

	// Delete all elements that are not the first and return
	while (v->count > 1) {
		dval_del(dval_pop(v, 1));
	}
	return v;
}

/* Returns Q-Expression of last element given a Q-Expression */
dval* builtin_last(denv* e, dval* a) {
	LASSERT_NUM("last", a, 1);
	LASSERT_MTYPE("last", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT_NOT_EMPTY("last", a, 0);

	// Take first argument
	dval* v = dval_take(a, 0);

	// Delete all elements that are not the last and return
	while (v->count > 1) {
		dval_del(dval_pop(v, 0));
	}
	return v;
}

/* Returns Q-Expression with first element removed given a Q-Expression */
dval* builtin_head(denv* e, dval* a) {
	LASSERT_NUM("head", a, 1);
	LASSERT_MTYPE("head", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT_NOT_EMPTY("head", a, 0);

	// Take first argument
	dval* v = dval_take(a, 0);

	dval_del(dval_pop(v, v->count - 1));
	return v;
}

/* Returns Q-Expression with last element removed given a Q-Expression*/
dval* builtin_tail(denv* e, dval* a) {
	LASSERT_NUM((char*) "tail", a, 1);
	LASSERT_MTYPE("tail", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST, \
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	LASSERT_NOT_EMPTY("tail", a, 0);

	dval* v = dval_take(a, 0);
	dval_del(dval_pop(v, 0));
	return v;
}

/* Returns a Q-Expression of given S-Expression */
dval* builtin_list(denv* e, dval* a) {
	a->type = DVAL_QEXPR;
	return a;
}

/* Evaluates a Q-Expression or List Literal as if it were an S-Expression */
dval* builtin_eval(denv* e, dval* a) {
	LASSERT_NUM("eval", a, 1);
	LASSERT_MTYPE("eval", a, 0, a->cell[0]->type == DVAL_QEXPR || a->cell[0]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));

	dval* x = dval_take(a, 0);
	x->type = DVAL_SEXPR;
	return dval_eval(e, x);
}

/** Evaluates the insides of a q-expression and returns the new q-expression.
  * Acts as if the q-expression is a list literal.
  */
dval* builtin_inner_eval(denv* e, dval* a) {
	LASSERT_NUM("inner_eval", a, 1);
	LASSERT_TYPE("inner_eval", a, 0, DVAL_QEXPR);

	dval* x = dval_take(a, 0);
	for (unsigned int i = 0; i < x->count; i++) {
		x->cell[i] = dval_eval(e, x->cell[i]);
	}
	return x;
}

dval* dval_call(denv* e, dval* f, dval* a) {
	if (f->builtin) {
		return f->builtin(e, a); // Delete f here???
	}

	int given = a->count;
	int total = f->formals->count;

	while (a->count) {
		if (f->formals->count == 0) {
			char* func_name = f->content->str;
			dval_del(a);
			dval_del(f);
			return dval_err((char*)"Function '%s' passed too many arguments. Got %i, Expected %i.",func_name, given, total);
		}

		dval* sym = dval_pop(f->formals, 0);

		if (strcmp(sym->content->str, "&") == 0) {
			if (f->formals->count != 1) {
				dval_del(a);
				dval_del(f);
				dval_del(sym);
				return dval_err((char*)"Function format invalid. Symbol '&' not followed by single symbol.");
			}

			dval* nsym = dval_pop(f->formals, 0);
			denv_put(f->env, nsym, builtin_list(e, a), 0);
			dval_del(sym); dval_del(nsym);
			break;
		}

		dval* val = dval_pop(a, 0);
		// Check if val is the correct type here:
		//
		denv_put(f->env, sym, val, 0);

		dval_del(sym); dval_del(val);
	}

	dval_del(a);

	if (f->formals->count > 0 && strcmp(f->formals->cell[0]->content->str, "&") == 0) {
		if (f->formals->count != 2) {
			return dval_err((char*)"Function format invalid. Symbol '&' not followed by single symbol.");
		}

		dval_del(dval_pop(f->formals, 0));

		dval* sym = dval_pop(f->formals, 0);
		dval* val = dval_qexpr();

		denv_put(f->env, sym, val, 0); // TODO: do differently?
		dval_del(sym); dval_del(val);
	}

	if (f->formals->count == 0) {
		f->env->par = e; // What is this used for???
		return builtin_eval(f->env, dval_add(dval_sexpr(), dval_copy(f->body))); // dval_del(f)???
	}
	else {
		return dval_copy(f);
	}
}

/* Returns Q-Expression joining one or more of given Q-Expressions or List Literals */
dval* builtin_join(denv* e, dval* a) { // TODO: Allow Strings
	for (unsigned int i = 0; i < a->count; i++) {
		LASSERT_MTYPE("join", a, i, a->cell[i]->type == DVAL_QEXPR || a->cell[i]->type == DVAL_LIST,
			"%s or %s.", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	}

	dval* x = dval_pop(a, 0);

	while (a->count) {
		x = dval_join(x, dval_pop(a, 0));
	}

	dval_del(a);
	x->type = DVAL_QEXPR; // Make sure what is returned is a qexpr, not a list
	return x;
}

dval* builtin_op(denv* e, dval* a, char* op) { // Make work with bytes!
	for (unsigned int i = 0; i < a->count; i++) {
		LASSERT_MTYPE(op, a, i, a->cell[i]->type == DDATA_INT || a->cell[i]->type == DDATA_DOUBLE,
			"%s or %s.", dtype_name(DDATA_INT), dtype_name(DDATA_DOUBLE));
	}

	dval* x = dval_pop(a, 0);
	int xIsDoub = x->type == DDATA_DOUBLE ? 1 : 0;

	if ((strcmp(op, "-") == 0) && a->count == 0) {
		if (xIsDoub) {
			x->content->doub = x->content->doub != 0 ? -x->content->doub : x->content->doub;
		}
		else {
			x->content->integer = x->content->integer != 0 ? -x->content->integer : x->content->integer;
		}
	}

	while (a->count > 0) {
		dval* y = dval_pop(a, 0);
		int yIsDoub = y->type == DDATA_DOUBLE ? 1 : 0;

		if (xIsDoub) {
			if (yIsDoub) {
				if (strcmp(op, "+") == 0) { x->content->doub += y->content->doub; }
				if (strcmp(op, "-") == 0) { x->content->doub -= y->content->doub; }
				if (strcmp(op, "*") == 0) { x->content->doub *= y->content->doub; }
				if (strcmp(op, "/") == 0) {
					if (y->content->doub == 0) {
						dval_del(x); dval_del(y);
						x = dval_err((char*)"Division By Zero!"); break;
					}
					x->content->doub /= y->content->doub;
				}
				if (strcmp(op, "%") == 0) {
					x = dval_err((char*)"Must use integers with modulus function.");
				}
				if (strcmp(op, "^") == 0) x->content->doub = pow(x->content->doub, y->content->doub);
			}
			else {
				if (strcmp(op, "+") == 0) { x->content->doub += (double)(y->content->integer); }
				if (strcmp(op, "-") == 0) { x->content->doub -= (double)(y->content->integer); }
				if (strcmp(op, "*") == 0) { x->content->doub *= (double)(y->content->integer); }
				if (strcmp(op, "/") == 0) {
					if (y->content->integer == 0) {
						dval_del(x); dval_del(y);
						x = dval_err((char*)"Division By Zero!"); break;
					}
					x->content->doub /= (double)(y->content->integer);
				}
				if (strcmp(op, "%") == 0) {
					x = dval_err((char*)"Must use integers with modulus function.");
				}
				if (strcmp(op, "^") == 0) x->content->doub = pow(x->content->doub, (double)y->content->integer);
			}
		}
		else {
			if (yIsDoub) {
				double xNum = (double)(x->content->integer);
				if (strcmp(op, "+") == 0) { x->content->doub = xNum + y->content->doub; }
				if (strcmp(op, "-") == 0) { x->content->doub = xNum - y->content->doub; }
				if (strcmp(op, "*") == 0) { x->content->doub = xNum * y->content->doub; }
				if (strcmp(op, "/") == 0) {
					if (y->content->doub == 0) {
						dval_del(x); dval_del(y);
						x = dval_err((char*)"Division By Zero!"); break;
					}
					x->content->doub = xNum / y->content->doub;
				}
				if (strcmp(op, "%") == 0) {
					x = dval_err((char*)"Must use integers with modulus function.");
				}
				if (strcmp(op, "^") == 0) x->content->doub = pow(xNum, y->content->doub);
				x->type = DDATA_DOUBLE;
				xIsDoub = 1;
			}
			else {
				if (strcmp(op, "+") == 0) { x->content->integer += y->content->integer; }
				if (strcmp(op, "-") == 0) { x->content->integer -= y->content->integer; }
				if (strcmp(op, "*") == 0) { x->content->integer *= y->content->integer; }
				if (strcmp(op, "/") == 0) {
					if (y->content->integer == 0) {
						dval_del(x); dval_del(y);
						x = dval_err((char*)"Division By Zero!"); break;
					}
					x->content->integer /= y->content->integer;
				}
				if (strcmp(op, "%") == 0) {
					if (y->content->integer == 0) {
						dval_del(x); dval_del(y);
						x = dval_err((char*)"Modulus By Zero!"); break;
					}
					x->content->integer %= y->content->integer;
				}
				if (strcmp(op, "^") == 0) x->content->integer = pow(x->content->integer, y->content->integer);
			}
		}
		dval_del(y);
	}

	dval_del(a);
	return x;
}

dval* builtin_var(denv* e, dval* a, char* func, int constant) {
	LASSERT_TYPE(func, a, 0, DVAL_QEXPR); // Allow List Literals???

	dval* syms = a->cell[0]; // syms: DVAL_QEXPR
	for (unsigned int i = 0; i < syms->count; i++) {
		LASSERT(a, syms->cell[i]->type == DVAL_SYM,
			(char*) "Function '%s' cannot define non-symbol. Got %s, Expected %s.", func,
			dtype_name(syms->cell[i]->type),
			dtype_name(DVAL_SYM));
	}

	LASSERT(a, (syms->count == a->count - 1),
		(char*) "Function '%s' passed too many arguments for symbols. Got %i, Expected %i.", func, syms->count, a->count - 1);

	for (unsigned int i = 0; i < syms->count; i++) { // For each of the symbols
		if (strcmp(func, "def") == 0) {
			denv_def(e, syms->cell[i], a->cell[i + 1], constant);
		} else if (strcmp(func, "let") == 0) {
			denv_put(e, syms->cell[i], a->cell[i + 1], constant);
		}
	}

	dval* result = dval_qexpr();
	result->count = a->count - 1;
	result->cell = (dval**)malloc(sizeof(dval*) * result->count);
	for (unsigned int i = 0; i < result->count; i++) {
		result->cell[i] = dval_copy(a->cell[i + 1]);
	}
	dval_del(a);
	return result;
}

dval* builtin_def(denv* e, dval* a) {
	return builtin_var(e, a, (char*)"def", 0);
}

dval* builtin_const(denv* e, dval* a) {
	return builtin_var(e, a, (char*) "def", 1);
}

dval* builtin_put(denv* e, dval* a) {
	return builtin_var(e, a, (char*) "let", 0);
}

dval* builtin_lambda(denv* e, dval* a) {
	LASSERT_NUM((char*) "\\", a, 2);
	LASSERT_TYPE((char*) "\\", a, 0, DVAL_QEXPR);
	LASSERT_MTYPE("\\", a, 1, a->cell[1]->type == DVAL_QEXPR || a->cell[1]->type == DVAL_LIST,
		"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
    LASSERT_NOT_EMPTY((char*) "\\", a, 0);

	dval* formals = dval_qexpr();

	for (unsigned int i = 0; i < a->cell[0]->count; i+=2) {
		LASSERT(a, (a->cell[0]->cell[i]->type == DVAL_SYM),
			(char*) "Cannot define non-symbol. Got %s, Expected %s.",
			dtype_name(a->cell[0]->cell[i]->type), dtype_name(DVAL_SYM));
        if (strcmp(a->cell[0]->cell[i]->content->str, "&") != 0) {
            if (i == a->cell[0]->count - 1) {
                //dval_del(a); TODO
                dval_del(formals);
                return dval_err("Symbol %s must have a type. Got NULL, Expected %s.", a->cell[0]->cell[i]->content->str, dtype_name(DVAL_NOTE));
            }
            if (a->cell[0]->cell[i+1]->type == DVAL_ERR) {
                dval* result = dval_pop(a->cell[0], i+1);
                dval_del(a);
                dval_del(formals);
                return result;
            }
            LASSERT(a, (a->cell[0]->cell[i+1]->type == DVAL_NOTE),
                (char*) "Symbol %s must have a type. Got %s, Expected %s.",
                a->cell[0]->cell[i]->content->str, dtype_name(a->cell[0]->cell[i+1]->type), dtype_name(DVAL_NOTE));
        } else {
            if (i == a->cell[0]->count - 1 || a->cell[0]->cell[i + 1]->type == DVAL_NOTE) {
                dval_del(a);
                dval_del(formals);
                return dval_err("An argument that takes multiple parameters must have a name!");
            } else if (i + 1 == a->cell[0]->count - 1) {
                // dval_del(a); TODO
                dval_del(formals);
                return dval_err("An argument that takes multiple parameters (`& %s`) must have a type!", a->cell[0]->cell[i+1]->content->str);
            }
        }

		dval* v = dval_copy(a->cell[0]->cell[i]);
        if (strcmp(v->content->str, "&") == 0) {
            if (i < a->cell[0]->count-3) {
                // dval_del(a); TODO
                dval_del(formals);
                return dval_err("An argument that takes multiple parameters (`& %s`) must be the last argument in the list!", a->cell[0]->cell[i+1]->content->str);
            }
            i--;
            // TODO: The type allowed in the list should be the type given for the argument (in its note).
            // Using type `any` will allow any type for the list.
        } else {
		  v->sym_type = a->cell[0]->cell[i]->content->type;
        }
		dval_add(formals, v);
		v = NULL;
	}

	dval* body = dval_pop(a, 1);
	dval_del(a);

    // TODO: Go through all of the items in the second q-expr (function body) and make sure that they all can evaluate to something (obviously skipping over the ones that use the function arguments)!
    // If not, give an error!

	return dval_lambda(formals, body);
}

dval* builtin_add(denv* e, dval* a) {
	return builtin_op(e, a, (char*)"+");
}

dval* builtin_sub(denv* e, dval* a) {
	return builtin_op(e, a, (char*)"-");
}

dval* builtin_mul(denv* e, dval* a) {
	return builtin_op(e, a, (char*)"*");
}

dval* builtin_div(denv* e, dval* a) {
	return builtin_op(e, a, (char*)"/");
}

dval* builtin_mod(denv* e, dval* a) {
	return builtin_op(e, a, (char*)"%");
}

dval* builtin_pow(denv* e, dval* a) {
	return builtin_op(e, a, (char*)"^");
}

dval* builtin_ord(denv* e, dval* a, char* op) { // TODO: Make work with bytes, strings, and characters!
	LASSERT_NUM(op, a, 2);
	LASSERT_MTYPE(op, a, 0, a->cell[0]->type == DDATA_INT || a->cell[0]->type == DDATA_DOUBLE || a->cell[0]->type == DDATA_BYTE || a->cell[0]->type == DDATA_CHAR, \
		"%s, %s, %s, or %s", dtype_name(DDATA_INT), dtype_name(DDATA_DOUBLE), dtype_name(DDATA_BYTE), dtype_name(DDATA_CHAR));
	LASSERT_MTYPE(op, a, 1, a->cell[1]->type == DDATA_INT || a->cell[1]->type == DDATA_DOUBLE || a->cell[1]->type == DDATA_BYTE || a->cell[1]->type == DDATA_CHAR, \
		"%s, %s, %s, or %s", dtype_name(DDATA_INT), dtype_name(DDATA_DOUBLE), dtype_name(DDATA_BYTE), dtype_name(DDATA_CHAR));
	//LASSERT_TYPE(op, a, 0, DDATA_INT || DDATA_DOUBLE || DDATA_BYTE || DDATA_CHAR);
	// LASSERT_TYPE(op, a, 1, DDATA_INT || DDATA_DOUBLE || DDATA_BYTE || DDATA_CHAR);

	int r;

	double num0 = a->cell[0]->type == DDATA_DOUBLE ? a->cell[0]->content->doub : a->cell[0]->content->integer; // TODO: Make this work with bytes!
	double num1 = a->cell[1]->type == DDATA_DOUBLE ? a->cell[1]->content->doub : a->cell[1]->content->integer;

	if (strcmp(op, ">") == 0) {
		r = num0 > num1;
	}
	if (strcmp(op, "<") == 0) {
		r = num0 < num1;
	}
	if (strcmp(op, ">=") == 0) {
		r = num0 >= num1;
	}
	if (strcmp(op, "<=") == 0) {
		r = num0 <= num1;
	}
	dval_del(a);
	return dval_int(r);
}

dval* builtin_gt(denv* e, dval* a) {
	return builtin_ord(e, a, (char*)">");
}

dval* builtin_lt(denv* e, dval* a) {
	return builtin_ord(e, a, (char*)"<");
}

dval* builtin_ge(denv* e, dval* a) {
	return builtin_ord(e, a, (char*)">=");
}

dval* builtin_le(denv* e, dval* a) {
	return builtin_ord(e, a, (char*)"<=");
}

dval* builtin_cmp(denv* e, dval* a, char* op) {
	LASSERT(a, a->count > 1, "You must have at least 2 arguments. Got %i, Expected 2+", a->count);
	int r;
	if (strcmp(op, "==") == 0) {
		for (unsigned int i = 1; i < a->count; i++) {
			if ((r = dval_eq(a->cell[0], a->cell[i]))) {
				break;
			}
		}
	}
	if (strcmp(op, "!=") == 0) {
		for (unsigned int i = 1; i < a->count; i++) {
			if (!(r = !dval_eq(a->cell[0], a->cell[i]))) {
				break;
			}
		}
	}
	dval_del(a);
	return dval_int(r);
}

/** Returns dval integer 1 (true) if the first item is equal to any of the other
  * items. Note that is does not have to be all of them, just at least one.
  * Ex: `(== (typeof x) integer double)` returns 1 if x is either an integer or a double
  */
dval* builtin_eq(denv* e, dval* a) {
	return builtin_cmp(e, a, (char*)"==");
}

/** Returns dval integer 1 (true) if the first item is not equal to all of the other
  * items.
  * Ex: `(!= (typeof x) integer double)` returns 1 if x is not an integer nor a double.
  */
dval* builtin_ne(denv* e, dval* a) {
	return builtin_cmp(e, a, (char*)"!=");
}

dval* builtin_not(denv* e, dval* a) {
	LASSERT_NUM("!", a, 1);
	LASSERT_TYPE("!", a, 0, DDATA_INT);

	dval* x;
	if (a->cell[0]->content->integer) {
		x = dval_int(0);
	}
	else {
		x = dval_int(1);
	}

	dval_del(a);
	return x;
}

dval* builtin_and(denv* e, dval* a) {
	for (unsigned int i = 0; i < a->count; i++) {
		LASSERT_TYPE((char*) "and", a, i, DDATA_INT);
	}

	int current = a->cell[0]->content->integer;
	if (current) {
		for (unsigned int i = 1; i < a->count; i++) {
			current = (current && a->cell[i]->content->integer);
		}
	}
	dval* x = dval_int(current);
	dval_del(a);
	return x;
}

dval* builtin_or(denv* e, dval* a) {
	for (unsigned int i = 0; i < a->count; i++) {
		LASSERT_TYPE((char*) "or", a, i, DDATA_INT);
	}

	int current = a->cell[0]->content->integer;
	for (unsigned int i = 1; i < a->count; i++) {
		current = (current || a->cell[i]->content->integer);
	}

	dval* x = dval_int(current);
	dval_del(a);
	return x;
}

/*dval* builtin_min(denv* e, dval* a) { // TODO


}*/

dval* builtin_if(denv* e, dval* a) { // Make work with bytes?
	if (a->count % 2 == 0) { // TODO: Make better later
		return dval_err((char*) "Must have a q-expression for each s-expression conditional, except for the else q-expression. The else q-expression is required.");
	}
	for (unsigned int i = 0; i < a->count-1; i += 2) {
		if (i == a->count - 1) break;
		LASSERT_MTYPE("if", a, i, a->cell[i]->type == DDATA_INT || a->cell[0]->type == DDATA_DOUBLE || a->cell[0]->type == DDATA_BYTE,
			"%s, %s, or %s", dtype_name(DDATA_INT), dtype_name(DDATA_DOUBLE), dtype_name(DDATA_BYTE));
		LASSERT_MTYPE("if", a, i+1, a->cell[i+1]->type == DVAL_QEXPR || a->cell[i+1]->type == DVAL_LIST,
			"%s or %s", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));
	}
	LASSERT_MTYPE("if", a, a->count-1, a->cell[a->count-1]->type == DVAL_QEXPR || a->cell[a->count-1]->type == DVAL_LIST,
		"%s or %s.", dtype_name(DVAL_QEXPR), dtype_name(DVAL_LIST));

	dval* x;
	for (unsigned int i = 0; i < a->count-1; i+=2) {
		a->cell[i+1]->type = DVAL_SEXPR;
		if (a->cell[i]->content->integer) {
			x = dval_eval(e, dval_pop(a, i + 1));
			dval_del(a);
			return x;
		}
	}
	a->cell[a->count-1]->type = DVAL_SEXPR;
	x = dval_eval(e, dval_pop(a, a->count-1));
	dval_del(a);
	return x;
}

dval* builtin_load(denv* e, dval* a) {
	LASSERT_NUM((char*) "load", a, 1);
	LASSERT_TYPE((char*) "load", a, 0, DDATA_STRING);

	mpc_result_t r;
	if (mpc_parse_contents(a->cell[0]->content->str, Line, &r)) {
		dval* expr = dval_read((mpc_ast_t*)r.output);
		mpc_ast_delete((mpc_ast_t*)r.output);

		while (expr->count) {
			dval* x = dval_eval(e, dval_pop(expr, 0));
			if (x->type == DVAL_ERR) {
				dval_println(x);
			}
			dval_del(x);
		}

		dval_del(expr);
		dval_del(a);

		return dval_sexpr();
	} else {
		char* err_msg = mpc_err_string(r.error);
		mpc_err_delete(r.error);

		dval* err = dval_err((char*)"Could not load Library %s", err_msg);
		free(err_msg);
		dval_del(a);

		return err;
	}
}

/*dval* builtin_concatenate(denv* e, dval* a) { // TODO
	for (int i = 0; i < a->count; i++) {
		LASSERT_TYPE("..", a, i, DDATA_STRING);
	}

}*/

dval* builtin_print(denv* e, dval* a) {
	for (unsigned int i = 0; i < a->count; i++) {
		dval_print(a->cell[i]); putchar(' ');
	}

	putchar('\n');
	dval_del(a);

	return dval_sexpr();
}

dval* builtin_error(denv* e, dval* a) {
	LASSERT_NUM((char*) "error", a, 1);
	LASSERT_TYPE((char*) "error", a, 0, DDATA_STRING);

	dval* err = dval_err(a->cell[0]->content->str);

	dval_del(a);
	return err;
}

dval* builtin_exit(denv* e, dval* a) {
	running = 0;
	return dval_int(1);
}

dval* dval_eval_sexpr(denv* e, dval* v) {
	for (unsigned int i = 0; i < v->count; i++) {
		v->cell[i] = dval_eval(e, v->cell[i]);
		if (v->cell[i]->type == DVAL_ERR) {
			return dval_take(v, i);
		}
	}

	if (v->count == 0) {
		return v;
	}

	if (v->count == 1) {
		return dval_eval(e, dval_take(v, 0));
	}

	dval* f = dval_pop(v, 0);
	if (f->type != DVAL_FUNC) {
		dval* err = dval_err(
			(char*)"S-Expression starts with incorrect type. Got %s, Expected %s.",
			dtype_name(f->type), dtype_name(DVAL_FUNC));
		dval_del(v);
		dval_del(f);
		return err;
	}

	dval* result = dval_call(e, f, v);
	dval_del(f);
	return result;
}

dval* dval_eval_qexpr(denv* e, dval* v) {
	for (unsigned int i = 0; i < v->count; i++) {
		if (v->cell[i]->type == DVAL_SLIST || v->cell[i]->type == DVAL_SSEXPR || v->cell[i]->type == DVAL_QEXPR) {
			v->cell[i] = dval_eval(e, v->cell[i]);
			if (v->cell[i]->type == DVAL_ERR) {
				return dval_take(v, i);
			}
		} else if (v->cell[i]->type == DVAL_LIST || v->cell[i]->type == DVAL_SEXPR) {
			v->cell[i] = dval_eval_qexpr(e, v->cell[i]);
			if (v->cell[i]->type == DVAL_ERR) {
				return dval_take(v, i);
			}
		}
	}

	return v;
}

dval* dval_eval_slist(denv* e, dval* v) {
	for (unsigned int i = 0; i < v->count; i++) {
		v->cell[i] = dval_eval(e, v->cell[i]);
		if (v->cell[i]->type == DVAL_ERR) {
			return dval_take(v, i);
		}
	}

	v->type = DVAL_LIST;
	return v;
}

dval* dval_eval_list(denv* e, dval* v) {
	for (unsigned int i = 0; i < v->count; i++) {
		v->cell[i] = dval_eval(e, v->cell[i]);
		if (v->cell[i]->type == DVAL_ERR) {
			return dval_take(v, i);
		}
	}

	return v;
}

dval* dval_eval(denv* e, dval* v) { // v is deleted!
	if (v->type == DVAL_SYM) {
		dval* x = denv_get(e, v);
		dval_del(v);
		return x;
	}
	if (v->type == DVAL_SEXPR) {
		return dval_eval_sexpr(e, v);
	} else if (v->type == DVAL_SSEXPR) {
		v->type = DVAL_SEXPR;
		return dval_eval_sexpr(e, v);
	} else if (v->type == DVAL_SQEXPR) {
		v->type = DVAL_QEXPR;
		return dval_eval_qexpr(e, v);
	} else if (v->type == DVAL_QEXPR) {
		return dval_eval_qexpr(e, v);
	} else if (v->type == DVAL_SLIST) {
		return dval_eval_slist(e, v);
	} else if (v->type == DVAL_LIST) {
		return dval_eval_list(e, v);
	}
	return v;
}

void denv_add_builtin(denv* e, char* name, dbuiltin func) {
	dval* k = dval_sym(name, DVAL_FUNC);
	dval* v = dval_func(func);
	denv_put(e, k, v, 0);
	dval_del(k); dval_del(v);
}

void denv_add_builtins(denv* e) {
	denv_add_builtin(e, (char*) "list", builtin_list);
	denv_add_builtin(e, (char*) "first", builtin_first);
	denv_add_builtin(e, (char*) "last", builtin_last);
	denv_add_builtin(e, (char*) "head", builtin_head);
	denv_add_builtin(e, (char*) "tail", builtin_tail);
	denv_add_builtin(e, (char*) "eval", builtin_eval);
	denv_add_builtin(e, (char*) "inner_eval", builtin_inner_eval);
	denv_add_builtin(e, (char*) "join", builtin_join);
	denv_add_builtin(e, (char*) "len", builtin_len);
	denv_add_builtin(e, (char*) "get", builtin_get);
	denv_add_builtin(e, (char*) "extract", builtin_extract);
	denv_add_builtin(e, (char*) "set", builtin_set);
	denv_add_builtin(e, (char*) "typeof", builtin_typeof);
	denv_add_builtin(e, (char*) "throw", builtin_throw);
	denv_add_builtin(e, (char*) "to_list", builtin_to_list); // TODO: to_qexpr
	denv_add_builtin(e, (char*) "to_qexpr", builtin_to_qexpr);

	denv_add_builtin(e, (char*) "while", builtin_while); // TODO

	denv_add_builtin(e, (char*) "+", builtin_add);
	denv_add_builtin(e, (char*) "-", builtin_sub);
	denv_add_builtin(e, (char*) "*", builtin_mul);
	denv_add_builtin(e, (char*) "/", builtin_div);
	denv_add_builtin(e, (char*) "%", builtin_mod);
	denv_add_builtin(e, (char*) "^", builtin_pow);

	denv_add_builtin(e, (char*) "def", builtin_def);
	denv_add_builtin(e, (char*) "const", builtin_const);
	denv_add_builtin(e, (char*) "let", builtin_put);
	denv_add_builtin(e, (char*) "\\", builtin_lambda);
	denv_add_builtin(e, (char*) "lambda", builtin_lambda);

	denv_add_builtin(e, (char*) "if", builtin_if);
	denv_add_builtin(e, (char*) "==", builtin_eq);
	denv_add_builtin(e, (char*) "!=", builtin_ne);
	denv_add_builtin(e, (char*) ">", builtin_gt);
	denv_add_builtin(e, (char*) "<", builtin_lt);
	denv_add_builtin(e, (char*) ">=", builtin_ge);
	denv_add_builtin(e, (char*) "<=", builtin_le);
	denv_add_builtin(e, (char*) "!", builtin_not);
	denv_add_builtin(e, (char*) "and", builtin_and);
	denv_add_builtin(e, (char*) "or", builtin_or);

	denv_add_builtin(e, (char*) "print", builtin_print);
	denv_add_builtin(e, (char*) "error", builtin_error);
	denv_add_builtin(e, (char*) "load", builtin_load);
	denv_add_builtin(e, (char*) "exit", builtin_exit);
}
