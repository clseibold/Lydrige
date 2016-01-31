#include "../headers/printing.h"

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

void dval_expr_print(dval* v, char* open, char* close) {
	for (unsigned int i = 0; i < strlen(open); i++) {
		putchar(open[i]);
	}
	for (unsigned int i = 0; i < v->count; i++) {
		dval_print(v->cell[i]);

		if (i != (v->count - 1)) {
			putchar(' ');
		}
	}
	for (unsigned int i = 0; i < strlen(open); i++) {
		putchar(close[i]);
	}
}

void dval_print_str(dval* v) {
	// char* escaped = (char*)malloc(strlen(v->content->str) + 1);
	// strcpy(escaped, v->content->str);
	// escaped = (char*)mpcf_escape(escaped);
	printf("%s", v->content->str);
	//free(escaped);
}

void dval_print_char(dval* v) {
	putchar(v->content->character);
}

void dval_print(dval* v) {
	switch (v->type) {
	case DDATA_RANGE:
		printf("%ld..%ld", v->content->integer, v->max);
		break;
	case DDATA_INT:
		printf("%ld", v->content->integer);
		break;
	case DDATA_DOUBLE:
		printf("%f", v->content->doub);
		break;
	case DDATA_BYTE:
		printf("0x%X", v->content->b);
		break;
	case DDATA_STRING:
		dval_print_str(v); break;
	case DDATA_CHAR:
		dval_print_char(v); break;
	case DVAL_ERR: printf("Error: %s", v->content->str); break;
	case DVAL_SYM: printf("%s", v->content->str); break;
	case DVAL_SLIST: dval_expr_print(v, (char*) "'[", (char*) "]"); break;
	case DVAL_LIST:  dval_expr_print(v, (char*) "[", (char*) "]"); break;
	case DVAL_SEXPR: dval_expr_print(v, (char*) "(", (char*) ")"); break;
	case DVAL_QEXPR: dval_expr_print(v, (char*) "{", (char*) "}"); break;
	case DVAL_FUNC:
		if (v->builtin) {
			printf("<builtin>");
		}
		else {
			printf("(\\ "); dval_print(v->formals);
			putchar(' '); dval_print(v->body); putchar(')');
		}
		break;
	}
}

void dval_println(dval* v) {
	dval_print(v);
	putchar('\n');
}
