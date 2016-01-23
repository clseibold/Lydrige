#include "../headers/reading.h"

dval* dval_read_int(mpc_ast_t* t) {
	errno = 0;
	long x = strtol(t->contents, NULL, 10);
	return errno != ERANGE ? dval_int(x) : dval_err("invalid integer");
}

dval* dval_read_double(mpc_ast_t* t) {
	errno = 0;
	double x = strtod(t->contents, NULL);
	return errno != ERANGE ? dval_double(x) : dval_err("invalid double");
}

dval* dval_read_byte(mpc_ast_t* t) {
	errno = 0;
	unsigned int hex = 0x00;
	sscanf(t->contents, "0x%02x", &hex);
	return errno != ERANGE ? dval_byte((byte) hex) : dval_err("invalid byte (hex)");
}

dval* dval_read_string(mpc_ast_t* t) {
	t->contents[strlen(t->contents) - 1] = '\0';
	char* unescaped = malloc(strlen(t->contents + 1) + 1);
	strcpy(unescaped, t->contents + 1);
	unescaped = mpcf_unescape(unescaped);
	dval* str = dval_string(unescaped);
	free(unescaped);
	return str;
}

dval* dval_read_character(mpc_ast_t* t) { // TODO
	return dval_sexpr(); // Change this eventually!
}

dval* dval_read(mpc_ast_t* t) {
	if (strstr(t->tag, "double")) {
		return dval_read_double(t);
	}
	if (strstr(t->tag, "integer")) {
		return dval_read_int(t);
	}
	if (strstr(t->tag, "byte")) {
		return dval_read_byte(t);
	}
	if (strstr(t->tag, "string")) {
		return dval_read_string(t);
	}
	/*if (strstr(t->tag, "character")) {
		return dval_read_character(t);
	}*/
	if (strstr(t->tag, "symbol")) {
		return dval_sym(t->contents);
	}

	dval* x = NULL;
	if (strcmp(t->tag, ">") == 0) {
		x = dval_sexpr();
	}
	if (strstr(t->tag, "sexpr")) {
		x = dval_sexpr();
	}
	if (strstr(t->tag, "qexpr")) {
		x = dval_qexpr();
	}

	for (int i = 0; i < t->children_num; i++) {
		if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
		if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
		if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
		if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
		if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
		if (strstr(t->children[i]->tag, "comment")) { continue; }
		x = dval_add(x, dval_read(t->children[i]));
	}

	return x;
}
