#include "../headers/structures.h"

char* dtype_name(int t) {
	switch (t) {
	case DVAL_FUNC: return (char*) "Function";
	case DDATA_RANGE: return (char*) "Range";
	case DDATA_INT: return (char*) "Integer";
	case DDATA_DOUBLE: return (char*) "Double";
	case DDATA_BYTE: return (char*) "Byte";
	case DDATA_CHAR: return (char*) "Char";
	case DDATA_STRING: return (char*) "String";
	case DVAL_ERR: return (char*) "Error";
	case DVAL_SYM: return (char*) "Symbol";
	case DVAL_LIST: return (char*) "List";
	case DVAL_SEXPR: return (char*) "S-Expression";
	case DVAL_QEXPR: return (char*) "Q-Expression";
	default: return (char*) "Unknown";
	}
}

/* Constructors */
denv* denv_new(void) {
	denv* e = (denv*)malloc(sizeof(denv));
	e->par = NULL;
	e->count = 0;
	/*e->syms = NULL;
	e->vals = NULL;*/
	if (!(e->hashtbl = hashtbl_create(1, NULL))) {
		fprintf(stderr, "Error: hashtbl_create() failed\n");
		exit(EXIT_FAILURE);
	}
	return e;
}

dval* dval_range(long min, long max) {
	dval* v = (dval*) malloc(sizeof(dval));
	v->type = DDATA_RANGE;
	v->content = (ddata*) malloc(sizeof(ddata));
	v->content->integer = min;
	v->max = max;

	return v;
}

dval* dval_int(long x) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_INT;
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->integer = x;

	return v;
}

dval* dval_double(double x) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_DOUBLE;
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->doub = x;
	return v;
}

dval* dval_byte(byte x) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_BYTE;
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->b = x;
	return v;
}

dval* dval_string(char* str) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_STRING;
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->str = (char*)malloc(strlen(str) + 1);
	strcpy(v->content->str, str);
	return v;
}

dval* dval_char(char character) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_CHAR;
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->character = character;
	return v;
}

dval* dval_err(char* fmt, ...) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_ERR;

	va_list va;
	va_start(va, fmt);
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->str = (char*)malloc(512);

	vsnprintf(v->content->str, 511, fmt, va);

	v->content->str = (char*)realloc(v->content->str, strlen(v->content->str) + 1);

	va_end(va);

	return v;
}

dval* dval_sym(char* s) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_SYM;
	v->content = (ddata*)malloc(sizeof(ddata));
	v->content->str = (char*)malloc(strlen(s) + 1);
	strcpy(v->content->str, s);
	return v;
}

dval* dval_usym(char* s) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_USYM;
	v->content->str = (char*)malloc(strlen(s) + 1);
	strcpy(v->content->str, s);
	return v;
}

dval* dval_sexpr(void) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

dval* dval_qexpr(void) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_QEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

dval* dval_list(void) {
	dval* v = (dval*) malloc(sizeof(dval));
	v->type = DVAL_LIST;
	v->count = 0;
	v->cell = NULL;
	return v;
}

dval* dval_slist(void) {
	dval* v = (dval*) malloc(sizeof(dval));
	v->type = DVAL_LIST;
	v->count = 0;
	v->cell = NULL;
	return v;
}

dval* dval_func(dbuiltin func) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_FUNC;
	v->builtin = func;
	return v;
}

dval* dval_lambda(dval* formals, dval* body) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_FUNC;

	v->builtin = NULL;
	v->env = denv_new();

	v->formals = formals;
	v->body = body;
	return v;
}

/* Destructors */
void denv_del(denv* e) {
	hashtbl_destroy(e->hashtbl);
	free(e);
}

void dval_del(dval* v) {
	switch (v->type) {
	case DVAL_FUNC:
		if (!v->builtin) {
			denv_del(v->env);
			dval_del(v->formals);
			dval_del(v->body);
		}
		break;
	case DDATA_RANGE:
	case DDATA_INT:
	case DDATA_BYTE:
	case DDATA_CHAR:
	case DDATA_DOUBLE: free(v->content); break;
	case DVAL_ERR:
	case DVAL_SYM:
	case DDATA_STRING: free(v->content->str); free(v->content); break;
	case DVAL_LIST:
	case DVAL_QEXPR:
	case DVAL_SEXPR:
		for (unsigned int i = 0; i < v->count; i++) {
			dval_del(v->cell[i]);
		}
		free(v->cell);
		break;
	}

	free(v);
}

dval* dval_add(dval* v, dval* x) {
	v->count++;
	v->cell = (dval**)realloc(v->cell, sizeof(dval*) * v->count);
	v->cell[v->count - 1] = x;
	return v;
}

/* Returns copy of given ddata */
union ddata* ddata_copy(int type, union ddata* d) {
	union ddata* x = (ddata*)malloc(sizeof(ddata));

	switch (type) {
	case DDATA_RANGE:
		x->integer = d->integer; break;
	case DDATA_INT:
		x->integer = d->integer; break;
	case DDATA_DOUBLE:
		x->doub = d->doub; break;
	case DDATA_BYTE:
		x->b = d->b; break;
	case DDATA_CHAR:
		x->character = d->character; break;
	case DVAL_ERR:
	case DVAL_SYM:
	case DDATA_STRING:
		x->str = (char*)malloc(strlen(d->str) + 1);
		strcpy(x->str, d->str); break;
	}

	return x;
}

/* Returns copy of given dval */
dval* dval_copy(dval* v) {
	dval* x = (dval*)malloc(sizeof(dval));
	x->type = v->type;

	switch (v->type) {
	case DVAL_FUNC: // TODO: Make sure this is correct!
		if (v->builtin) {
			x->builtin = v->builtin;
		}
		else {
			x->builtin = NULL;
			x->env = denv_copy(v->env);
			x->formals = dval_copy(v->formals);
			x->body = dval_copy(v->body);
		}
		break;
	case DDATA_RANGE:
		x->max = v->max;
	case DDATA_BYTE:
	case DDATA_DOUBLE:
	case DDATA_INT:
	case DDATA_CHAR:
	case DDATA_STRING:
		x->content = ddata_copy(x->type, v->content);
		break;
	case DVAL_ERR:
		x->content = ddata_copy(x->type, v->content);
		break;
	case DVAL_SYM:
		x->content = ddata_copy(x->type, v->content);
		x->constant = v->constant;
		break;
	case DVAL_LIST:
	case DVAL_SEXPR:
	case DVAL_QEXPR:
		x->count = v->count;
		x->cell = (dval**)malloc(sizeof(dval*) * v->count);
		for (unsigned int i = 0; i < v->count; i++) {
			x->cell[i] = dval_copy(v->cell[i]);
		}
		break;
	}
	return x;
}

denv* denv_copy(denv* e) { // There may be a problem with this
	denv* n = (denv*)malloc(sizeof(denv));
	n->par = e->par;
	n->count = e->count;
	//n->hashtbl = hashtbl_create(0, NULL);
	// TODO: Go through each node in e's hashtable and insert into n's hashtable!
	n->hashtbl = hashtbl_copy(e->hashtbl);
	return n;
}
