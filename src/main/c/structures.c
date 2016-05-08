#include "../headers/structures.h"

int running = 1;

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
	case DVAL_TYPE: return (char*) "Type";
	case DVAL_LIST: return (char*) "List";
	case DVAL_SLIST: return (char*) "Special List";
	case DVAL_SEXPR: return (char*) "S-Expression";
	case DVAL_SSEXPR: return (char*) "Special S-Expression";
	case DVAL_QEXPR: return (char*) "Q-Expression";
	case DVAL_NOTE: return (char*) "Note";
	case DDATA_ANY: return (char*) "Any"; // TODO: Do not allow people to use 'any' unless in declarations!!!
	default: return (char*) "Unknown";
	}
}

/* Constructors */
denv* denv_new(void) {
	denv* e = (denv*)malloc(sizeof(denv));
	e->par = NULL;
	e->count = 0;
	e->map = Hashmap_create(NULL, NULL);
	if (e->map == NULL) {
		fprintf(stderr, "ERROR: Failed to create hashmap!");
		exit(EXIT_FAILURE);
	}
	//if (!(e->hashtbl = hashtbl_create(1, NULL))) {
	//	fprintf(stderr, "Error: hashtbl_create() failed\n");
	//	exit(EXIT_FAILURE);
	//}
	return e;
}

dval* dval_range(long min, long max) {
	dval* v = (dval*) malloc(sizeof(dval));
	v->type = DDATA_RANGE;
	v->integer = min;
	v->max = max;

	return v;
}

dval* dval_int(long x) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_INT;
	v->integer = x;

	return v;
}

dval* dval_double(double x) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_DOUBLE;
	v->doub = x;
	return v;
}

dval* dval_byte(byte x) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_BYTE;
	v->b = x;
	return v;
}

dval* dval_string(char* str) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_STRING;
	v->str = (char*)malloc(strlen(str) + 1);
	strcpy(v->str, str);
	return v;
}

dval* dval_char(char character) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DDATA_CHAR;
	v->character = character;
	return v;
}

dval* dval_err(char* fmt, ...) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_ERR;

	va_list va;
	va_start(va, fmt);
	v->str = (char*)malloc(512);

	vsnprintf(v->str, 511, fmt, va);

	v->str = (char*)realloc(v->str, strlen(v->str) + 1);

	va_end(va);

	return v;
}

dval* dval_sym(char* s, int type) {
	if (strcmp(s, "int") == 0) return dval_type(DDATA_INT);
	else if (strcmp(s, "double") == 0) return dval_type(DDATA_DOUBLE);
	else if (strcmp(s, "byte") == 0) return dval_type(DDATA_BYTE);
	else if (strcmp(s, "string") == 0) return dval_type(DDATA_STRING);
	else if (strcmp(s, "char") == 0) return dval_type(DDATA_CHAR);
	else if (strcmp(s, "err") == 0) return dval_type(DVAL_ERR);
	else if (strcmp(s, "qexpr") == 0) return dval_type(DVAL_QEXPR);
	else if (strcmp(s, "lst") == 0) return dval_type(DVAL_LIST);
	else if (strcmp(s, "func") == 0) return dval_type(DVAL_FUNC);
	else if (strcmp(s, "any") == 0) return dval_type(DDATA_ANY);

	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_SYM;
	v->sym_type = type;
	v->str = (char*)malloc(strlen(s) + 1);
	strcpy(v->str, s);
	return v;
}

dval* dval_type(int type) {
	dval* v = (dval*) malloc(sizeof(dval));
	v->type = DVAL_TYPE;
	v->ttype = type;
	return v;
}

dval* dval_sexpr(void) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

dval* dval_ssexpr(void) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_SSEXPR;
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

dval* dval_sqexpr(void) {
	dval* v = (dval*)malloc(sizeof(dval));
	v->type = DVAL_SQEXPR;
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
	v->type = DVAL_SLIST;
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

dval* dval_note(void) {
	dval* v = (dval*) malloc(sizeof(dval));
	v->type = DVAL_NOTE;
	v->count = 0;
	v->cell = NULL;
	return v;
}

/* Destructors */
void denv_del(denv* e) {
	Hashmap_destroy(e->map);
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
	case DVAL_ERR:
	case DVAL_SYM:
	case DDATA_STRING: free(v->str); break;
	case DVAL_LIST:
	case DVAL_SLIST:
	case DVAL_SQEXPR:
	case DVAL_QEXPR:
	case DVAL_SSEXPR:
	case DVAL_NOTE:
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

/* Modifies x to have the same ddata content as d */
dval* ddata_copy(int type, dval* d, dval* x) {
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
	case DVAL_TYPE:
		x->ttype = d->ttype; break;
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
	case DVAL_TYPE:
	case DDATA_STRING:
		x = ddata_copy(x->type, v, x);
		break;
	case DVAL_ERR:
		x = ddata_copy(x->type, v, x);
		break;
	case DVAL_SYM:
		x = ddata_copy(x->type, v, x);
		x->sym_type = v->sym_type;
		x->constant = v->constant;
		break;
	case DVAL_LIST:
	case DVAL_SLIST:
	case DVAL_SEXPR:
	case DVAL_SSEXPR:
	case DVAL_SQEXPR:
	case DVAL_NOTE:
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
	// TODO: Get each node in hashmap and copy the value, put into new hashmap
	denv* result = denv_new();
	result->count = e->count;
	if (result->par) {
		result->par = e->par;
	}
	Hashmap_destroy(result->map);
	result->map = Hashmap_copy(e->map);

	return result;
}
