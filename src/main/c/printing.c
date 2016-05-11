#include "../headers/printing.h"

#include <ctype.h>
#include <stdint.h>

internal void dval_expr_print(dval* v, char* open, char* close) {
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

internal void dval_print_str(dval* v) {
	// char* escaped = (char*)malloc(strlen(v->str) + 1);
	// strcpy(escaped, v->str);
	// escaped = (char*)mpcf_escape(escaped);
	printf("%s", v->str);
	//free(escaped);
}

internal void dval_print_char(dval* v) {
	putchar(v->character);
}

void dval_print(dval* v) {
	switch (v->type) {
	case DDATA_RANGE:
		printf("%ld..%ld", v->integer, v->max);
		break;
	case DDATA_INT:
		printf("%ld", v->integer);
		break;
	case DDATA_DOUBLE:
		printf("%f", v->doub);
		break;
	case DDATA_BYTE:
		printf("0x%X", v->b);
		break;
	case DVAL_TYPE:
		printf("%s", dtype_name(v->ttype)); break;
	case DDATA_STRING:
		dval_print_str(v); break;
	case DDATA_CHAR:
		dval_print_char(v); break;
	case DVAL_ERR: printf("Error: %s", v->str); break;
	case DVAL_SYM: printf("%s", v->str); break;
	case DVAL_NOTE: dval_expr_print(v, (char*) ": (", (char*) ")"); break;
	case DVAL_SLIST: dval_expr_print(v, (char*) "[", (char*) "]"); break;
	case DVAL_LIST:  dval_expr_print(v, (char*) "[", (char*) "]"); break;
	case DVAL_SSEXPR: dval_expr_print(v, (char*) "(", (char*) ")"); break;
	case DVAL_SEXPR: dval_expr_print(v, (char*) "(", (char*) ")"); break;
	case DVAL_SQEXPR: dval_expr_print(v, (char*) "{", (char*) "}"); break;
	case DVAL_QEXPR: dval_expr_print(v, (char*) "{", (char*) "}"); break;
	case DVAL_FUNC:
		if (v->builtin) {
			printf("<builtin>");
		}
		else {
			dval_print(v->formals);
			printf(" -> "); dval_print(v->body);
		}
		break;
	default:
		printf("UNKNOWN\n");
		break;
	}
}

void dval_println(dval* v) {
	dval_print(v);
	putchar('\n');
}
