#include "../headers/builtin.h"

/* Returns extracted element at index i, shifts list elements backwards to fill in where element was at */
dval* dval_pop(dval* v, int i) {
	dval* x = v->cell[i];
	memmove(&v->cell[i], &v->cell[i + 1],
		sizeof(dval*) * (v->count - i - 1));

	v->count--;

	v->cell = realloc(v->cell, sizeof(dval*) * v->count);
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

int dval_eq(dval* x, dval* y) {
	if (x->type != y->type) {
		return 0;
	}

	switch (x->type) {
	case DDATA_INT:
		return (x->content->integer == y->content->integer);
	case DDATA_DOUBLE:
		return (x->content->doub == y->content->doub);
	case DDATA_BYTE:
		return (x->content->byte == y->content->byte);
	case DDATA_STRING:
		return (strcmp(x->content->str, y->content->str) == 0);
	case DVAL_ERR:
		return (strcmp(x->content->str, y->content->str) == 0);
	case DVAL_SYM:
		return (strcmp(x->content->str, y->content->str) == 0);
	case DVAL_FUNC:
		if (x->builtin || y->builtin) {
			return x->builtin == y->builtin;
		}
		else {
			return dval_eq(x->formals, y->formals) && dval_eq(x->body, y->body);
		}
	case DVAL_QEXPR:
	case DVAL_SEXPR:
		if (x->count != y->count) {
			return 0;
		}
		for (int i = 0; i < x->count; i++) {
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
	if (d = hashtbl_get(e->hashtbl, k->content->str)) {
		return dval_copy(d);
	}

	if (e->par) {
		return denv_get(e->par, k);
	}
	// TODO: Return unbound symbol type!
	return dval_err("Unbound symbol '%s'", k->content->str);
	// dval_usym(k->sym);
}

void denv_put(denv* e, dval* k, dval* v, int constant) { // TODO: doesn't work correctly
	dval* t;
	if (hashtbl_get(e->hashtbl, k->content->str)) {
		if (((dval*)hashtbl_get(e->hashtbl, k->content->str))->constant == 0) {
			hashtbl_remove(e->hashtbl, k->content->str);
			t = dval_copy(v);
			t->constant = constant;
			hashtbl_insert(e->hashtbl, k->content->str, t);
			return;
		}
		else {
			printf("Error: Cannot edit '%s'. It is a constant\n", k->content->str);
			return;
		}
	}

	e->count++;
	hashtbl_resize(e->hashtbl, e->hashtbl->size + 1); // TODO: Make this more efficient?
	t = dval_copy(v);
	t->constant = constant;
	hashtbl_insert(e->hashtbl, k->content->str, t);
	return;
}

void denv_def(denv* e, dval* k, dval* v, int constant) {
	while (e->par) {
		e = e->par;
	}
	denv_put(e, k, v, constant);
}

/* Returns length of given Q-Expression */
dval* builtin_len(denv* e, dval* a) {
	LASSERT(a, a->count == 1,
		"Funcion 'eval' passed too many arguments.");
	LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
		"Function 'eval' passed incorrect types.");

	dval* x = dval_int(a->cell[0]->count);
	dval_del(a);
	return x;
}

/* Returns Q-Expression of first element given a Q-Expression */
dval* builtin_first(denv* e, dval* a) {
	LASSERT(a, a->count == 1,
		"Funcion 'first' passed too many arguments.");
	LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
		"Function 'first' passed incorrect types.");
	LASSERT(a, a->cell[0]->count != 0,
		"Function 'first' passed {}.");

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
	LASSERT(a, a->count == 1,
		"Funcion 'first' passed too many arguments.");
	LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
		"Function 'first' passed incorrect types.");
	LASSERT(a, a->cell[0]->count != 0,
		"Function 'first' passed {}.");

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
	LASSERT(a, a->count == 1,
		"Funcion 'tail' passed too many arguments.");
	LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
		"Function 'tail' passed incorrect types.");
	LASSERT(a, a->cell[0]->count != 0,
		"Function 'tail' passed {}.");

	// Take first argument
	dval* v = dval_take(a, 0);

	dval_del(dval_pop(v, v->count - 1));
	return v;
}

/* Returns Q-Expression with last element removed given a Q-Expression*/
dval* builtin_tail(denv* e, dval* a) {
	LASSERT(a, a->count == 1,
		"Funcion 'tail' passed too many arguments.");
	LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
		"Function 'tail' passed incorrect types.");
	LASSERT(a, a->cell[0]->count != 0,
		"Function 'tail' passed {}.");

	dval* v = dval_take(a, 0);
	dval_del(dval_pop(v, 0));
	return v;
}

/* Returns a Q-Expression of given S-Expression */
dval* builtin_list(denv* e, dval* a) {
	a->type = DVAL_QEXPR;
	return a;
}

/* Evaluates a Q-Expression as if it were an S-Expression */
dval* builtin_eval(denv* e, dval* a) {
	LASSERT(a, a->count == 1,
		"Funcion 'eval' passed too many arguments.");
	LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
		"Function 'eval' passed incorrect types.");

	dval* x = dval_take(a, 0);
	x->type = DVAL_SEXPR;
	return dval_eval(e, x);
}

dval* dval_call(denv* e, dval* f, dval* a) {
	if (f->builtin) {
		return f->builtin(e, a);
	}

	int given = a->count;
	int total = f->formals->count;

	while (a->count) {
		if (f->formals->count == 0) {
			dval_del(a); return dval_err(
				"Function passed too many arguments. Got %i, Expected %i.", given, total);
		}

		dval* sym = dval_pop(f->formals, 0);

		if (strcmp(sym->content->str, "&") == 0) {
			if (f->formals->count != 1) {
				dval_del(a);
				return dval_err("Function format invalid. Symbol '&' not followed by single symbol.");
			}

			dval* nsym = dval_pop(f->formals, 0);
			denv_put(f->env, nsym, builtin_list(e, a), 0); // TODO: do differently?
			dval_del(sym); dval_del(nsym);
			break;
		}

		dval* val = dval_pop(a, 0);
		denv_put(f->env, sym, val, 0); // TODO: do differently?

		dval_del(sym); dval_del(val);
	}

	dval_del(a);

	if (f->formals->count > 0 && strcmp(f->formals->cell[0]->content->str, "&") == 0) {
		if (f->formals->count != 2) {
			return dval_err("Function format invalid. Symbol '&' not followed by single symbol.");
		}

		dval_del(dval_pop(f->formals, 0));

		dval* sym = dval_pop(f->formals, 0);
		dval* val = dval_qexpr();

		denv_put(f->env, sym, val, 0); // TODO: do differently?
		dval_del(sym); dval_del(val);
	}

	if (f->formals->count == 0) {
		f->env->par = e;
		return builtin_eval(f->env, dval_add(dval_sexpr(), dval_copy(f->body)));
	}
	else {
		return dval_copy(f);
	}
}

/* Returns Q-Expression joining one or more of given Q-Expressions */
dval* builtin_join(denv* e, dval* a) {
	for (int i = 0; i < a->count; i++) {
		LASSERT(a, a->cell[0]->type == DVAL_QEXPR,
			"Function 'join' passed incorrect type.");
	}

	dval* x = dval_pop(a, 0);

	while (a->count) {
		x = dval_join(x, dval_pop(a, 0));
	}

	dval_del(a);
	return x;
}

dval* builtin_op(denv* e, dval* a, char* op) { // Make work with bytes!
	for (int i = 0; i < a->count; i++) {
		if ((a->cell[i]->type != DDATA_INT) && a->cell[i]->type != DDATA_DOUBLE) {
			dval_del(a);
			return dval_err("Cannot operate on non-number!");
		}
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
						x = dval_err("Division By Zero!"); break;
					}
					x->content->doub /= y->content->doub;
				}
				if (strcmp(op, "%") == 0) {
					x = dval_err("Must use integers with modulus function.");
				}
			}
			else {
				if (strcmp(op, "+") == 0) { x->content->doub += (double)(y->content->integer); }
				if (strcmp(op, "-") == 0) { x->content->doub -= (double)(y->content->integer); }
				if (strcmp(op, "*") == 0) { x->content->doub *= (double)(y->content->integer); }
				if (strcmp(op, "/") == 0) {
					if (y->content->integer == 0) {
						dval_del(x); dval_del(y);
						x = dval_err("Division By Zero!"); break;
					}
					x->content->doub /= (double)(y->content->integer);
				}
				if (strcmp(op, "%") == 0) {
					x = dval_err("Must use integers with modulus function.");
				}
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
						x = dval_err("Division By Zero!"); break;
					}
					x->content->doub = xNum / y->content->doub;
				}
				if (strcmp(op, "%") == 0) {
					x = dval_err("Must use integers with modulus function.");
				}
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
						x = dval_err("Division By Zero!"); break;
					}
					x->content->integer /= y->content->integer;
				}
				if (strcmp(op, "%") == 0) {
					if (y->content->integer == 0) {
						dval_del(x); dval_del(y);
						x = dval_err("Modulus By Zero!"); break;
					}
					x->content->integer %= y->content->integer;
				}
			}
		}
		dval_del(y);
	}

	dval_del(a);
	return x;
}

dval* builtin_var(denv* e, dval* a, char* func) {
	LASSERT_TYPE(func, a, 0, DVAL_QEXPR);

	dval* syms = a->cell[0];
	for (int i = 0; i < syms->count; i++) {
		LASSERT(a, (syms->cell[i]->type == DVAL_SYM),
			"Function '%s' cannot define non-symbol. Got %s, Expected %s.", func,
			dtype_name(syms->cell[i]->type),
			dtype_name(DVAL_SYM));
	}

	LASSERT(a, (syms->count == a->count - 1),
		"Function '%s' passed too many arguments for symbols. Got %i, Expected %i.", func, syms->count, a->count - 1);

	for (int i = 0; i < syms->count; i++) {
		if (strcmp(func, "def") == 0) {
			denv_def(e, syms->cell[i], a->cell[i + 1], 0);
		}

		if (strcmp(func, "=") == 0) {
			denv_put(e, syms->cell[i], a->cell[i + 1], 0);
		}
	}

	dval_del(a);
	return dval_sexpr();
}

dval* builtin_def(denv* e, dval* a) {
	return builtin_var(e, a, "def");
}

dval* builtin_const(denv* e, dval* a) { // TODO
	//LASSERT_NUM("const", a, 2); // const func {varNameAsUSYM} varValue, func - def or =
	// LASSERT_TYPE("const", a, 0, DVAL_FUNC);
	LASSERT_TYPE("const", a, 0, DVAL_QEXPR); // TODO: do differently?

	dval* syms = a->cell[0];
	for (int i = 0; i < syms->count; i++) {
		LASSERT(a, (syms->cell[i]->type == DVAL_SYM),
			"Function '%s' cannot define non-symbol. Got %s, Expected %s.", "const",
			dtype_name(syms->cell[i]->type),
			dtype_name(DVAL_SYM));
	}

	LASSERT(a, (syms->count == a->count - 1),
		"Function '%s' passed too many arguments for symbols. Got %i, Expected %i.", "const", syms->count, a->count - 1);

	for (int i = 0; i < syms->count; i++) {
		if (strcmp("def", "def") == 0) {
			denv_put(e, syms->cell[i], a->cell[i + 1], 1); // TODO: do differently?
		}
	}

	dval_del(a);
	return dval_sexpr();
}

dval* builtin_put(denv* e, dval* a) {
	return builtin_var(e, a, "=");
}

dval* builtin_lambda(denv* e, dval* a) {
	LASSERT_NUM("\\", a, 2);
	LASSERT_TYPE("\\", a, 0, DVAL_QEXPR);
	LASSERT_TYPE("\\", a, 1, DVAL_QEXPR);

	for (int i = 0; i < a->cell[0]->count; i++) {
		LASSERT(a, (a->cell[0]->cell[i]->type == DVAL_SYM),
			"Cannot define non-symbol. Got %s, Expected %s.",
			dtype_name(a->cell[0]->cell[i]->type), dtype_name(DVAL_SYM));
	}

	dval* formals = dval_pop(a, 0);
	dval* body = dval_pop(a, 0);
	dval_del(a);

	return dval_lambda(formals, body);
}

dval* builtin_add(denv* e, dval* a) {
	return builtin_op(e, a, "+");
}

dval* builtin_sub(denv* e, dval* a) {
	return builtin_op(e, a, "-");
}

dval* builtin_mul(denv* e, dval* a) {
	return builtin_op(e, a, "*");
}

dval* builtin_div(denv* e, dval* a) {
	return builtin_op(e, a, "/");
}

dval* builtin_mod(denv* e, dval* a) {
	return builtin_op(e, a, "%");
}

dval* builtin_ord(denv* e, dval* a, char* op) { // TODO: Make work with bytes, strings, and characters!
	LASSERT_NUM(op, a, 2);
	LASSERT_TYPE(op, a, 0, DDATA_INT || DDATA_DOUBLE || DDATA_BYTE);
	LASSERT_TYPE(op, a, 1, DDATA_INT || DDATA_DOUBLE || DDATA_BYTE);

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
	return builtin_ord(e, a, ">");
}

dval* builtin_lt(denv* e, dval* a) {
	return builtin_ord(e, a, "<");
}

dval* builtin_ge(denv* e, dval* a) {
	return builtin_ord(e, a, ">=");
}

dval* builtin_le(denv* e, dval* a) {
	return builtin_ord(e, a, "<=");
}

dval* builtin_cmp(denv* e, dval* a, char* op) {
	LASSERT_NUM(op, a, 2);
	int r;
	if (strcmp(op, "==") == 0) {
		r = dval_eq(a->cell[0], a->cell[1]);
	}
	if (strcmp(op, "!=") == 0) {
		r = !dval_eq(a->cell[0], a->cell[1]);
	}
	dval_del(a);
	return dval_int(r);
}

dval* builtin_eq(denv* e, dval* a) {
	return builtin_cmp(e, a, "==");
}

dval* builtin_ne(denv* e, dval* a) {
	return builtin_cmp(e, a, "!=");
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
	for (int i = 0; i < a->count; i++) {
		LASSERT_TYPE("and", a, a->cell[i]->type, DDATA_INT);
	}

	int current = a->cell[0]->content->integer;
	if (current) {
		for (int i = 1; i < a->count; i++) {
			current = (current && a->cell[i]->content->integer);
		}
	}
	dval* x = dval_int(current);
	dval_del(a);
	return x;
}

dval* builtin_or(denv* e, dval* a) {
	for (int i = 0; i < a->count; i++) {
		LASSERT_TYPE("or", a, a->cell[i]->type, DDATA_INT);
	}

	int current = a->cell[0]->content->integer;
	for (int i = 1; i < a->count; i++) {
		current = (current || a->cell[i]->content->integer);
	}

	dval* x = dval_int(current);
	dval_del(a);
	return x;
}

/*dval* builtin_min(denv* e, dval* a) { // TODO
	

}*/

dval* builtin_if(denv* e, dval* a) { // Make work with bytes!
	LASSERT_NUM("if", a, 3);
	LASSERT_TYPE("if", a, 0, DDATA_INT || DDATA_DOUBLE || DDATA_BYTE);
	LASSERT_TYPE("if", a, 1, DVAL_QEXPR);
	LASSERT_TYPE("if", a, 2, DVAL_QEXPR);

	dval* x;
	a->cell[1]->type = DVAL_SEXPR;
	a->cell[2]->type = DVAL_SEXPR;

	if (a->cell[0]->content->integer) {
		x = dval_eval(e, dval_pop(a, 1));
	}
	else {
		x = dval_eval(e, dval_pop(a, 2));
	}

	dval_del(a);
	return x;
}

dval* builtin_load(denv* e, dval* a) {
	LASSERT_NUM("load", a, 1);
	LASSERT_TYPE("load", a, 0, DDATA_STRING);

	mpc_result_t r;
	if (mpc_parse_contents(a->cell[0]->content->str, Line, &r)) {
		dval* expr = dval_read(r.output);
		mpc_ast_delete(r.output);

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
	}
	else {
		char* err_msg = mpc_err_string(r.output);
		mpc_err_delete(r.output);

		dval* err = dval_err("Could not load Library %s", err_msg);
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
	for (int i = 0; i < a->count; i++) {
		dval_print(a->cell[i]); putchar(' ');
	}

	putchar('\n');
	dval_del(a);

	return dval_sexpr();
}

dval* builtin_error(denv* e, dval* a) {
	LASSERT_NUM("error", a, 1);
	LASSERT_TYPE("error", a, 0, DDATA_STRING);

	dval* err = dval_err(a->cell[0]->content->str);

	dval_del(a);
	return err;
}

dval* builtin_exit(denv* e, dval* a) {
	running = 0;
	return dval_int(1);
}

dval* dval_eval_sexpr(denv* e, dval* v) {
	for (int i = 0; i < v->count; i++) {
		v->cell[i] = dval_eval(e, v->cell[i]);
	}

	for (int i = 0; i < v->count; i++) {
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
			"S-Expression starts with incorrect type. Got %s, Expected %s.",
			dtype_name(f->type), dtype_name(DVAL_FUNC));
		dval_del(v); dval_del(f);
		return err;
	}

	dval* result = dval_call(e, f, v);
	dval_del(f);
	return result;
}

dval* dval_eval(denv* e, dval* v) {
	if (v->type == DVAL_SYM) {
		dval* x = denv_get(e, v);
		dval_del(v);
		return x;
	}
	if (v->type == DVAL_SEXPR) {
		return dval_eval_sexpr(e, v);
	}
	return v;
}

void denv_add_builtin(denv* e, char* name, dbuiltin func) {
	dval* k = dval_sym(name);
	dval* v = dval_func(func);
	denv_put(e, k, v, 0);
	dval_del(k); dval_del(v);
}

void denv_add_builtins(denv* e) {
	denv_add_builtin(e, "list", builtin_list);
	denv_add_builtin(e, "first", builtin_first);
	denv_add_builtin(e, "last", builtin_last);
	denv_add_builtin(e, "head", builtin_head);
	denv_add_builtin(e, "tail", builtin_tail);
	denv_add_builtin(e, "eval", builtin_eval);
	denv_add_builtin(e, "join", builtin_join);
	denv_add_builtin(e, "len", builtin_len);

	denv_add_builtin(e, "+", builtin_add);
	denv_add_builtin(e, "-", builtin_sub);
	denv_add_builtin(e, "*", builtin_mul);
	denv_add_builtin(e, "/", builtin_div);
	denv_add_builtin(e, "%", builtin_mod);

	denv_add_builtin(e, "def", builtin_def);
	denv_add_builtin(e, "const", builtin_const); // TODO
	denv_add_builtin(e, "=", builtin_put);
	denv_add_builtin(e, "\\", builtin_lambda);
	denv_add_builtin(e, "lambda", builtin_lambda);

	denv_add_builtin(e, "if", builtin_if);
	denv_add_builtin(e, "==", builtin_eq);
	denv_add_builtin(e, "!=", builtin_ne);
	denv_add_builtin(e, ">", builtin_gt);
	denv_add_builtin(e, "<", builtin_lt);
	denv_add_builtin(e, ">=", builtin_ge);
	denv_add_builtin(e, "<=", builtin_le);
	denv_add_builtin(e, "!", builtin_not);
	denv_add_builtin(e, "and", builtin_and);
	denv_add_builtin(e, "or", builtin_or);

	denv_add_builtin(e, "print", builtin_print);
	denv_add_builtin(e, "error", builtin_error);
	denv_add_builtin(e, "load", builtin_load);
	denv_add_builtin(e, "exit", builtin_exit);
}
