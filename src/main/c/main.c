#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <readline/readline.h>

#include "../headers/hashtable.h"
#include "../../mpc/headers/mpc.h"
#include "../headers/structures.h"
#include "../headers/reading.h"
#include "../headers/printing.h"
#include "../headers/builtin.h"

/*
static char buffer[2048];

char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1] = '\0';
	return cpy;
}
*/

int main(int argc, char** argv) {
	Expr = mpc_new("expr");
	Data = mpc_new("data");
	Double = mpc_new("double");
	Integer = mpc_new("integer");
	Byte = mpc_new("byte");
	Comment = mpc_new("comment");
	String = mpc_new("string");
	Character = mpc_new("character");
	Symbol = mpc_new("symbol");
	Sexpr = mpc_new("sexpr");
	Qexpr = mpc_new("qexpr");
	Line = mpc_new("line");

	mpca_lang(MPCA_LANG_DEFAULT,
		" \
		expr : <data> | <symbol> | <sexpr> | <qexpr> | <comment> ; \
		data : <byte> | <double> | <integer> | <string> | <character> ; \
		double : /-?[0-9]+\\.[0-9]+/ ; \
		integer : /-?[0-9]+/ ; \
		byte : /0x[0-9a-zA-Z][0-9a-zA-Z]/ ; \
		string : /\"(\\\\.|[^\"])*\"/ ; \
		character : /\'(\\\\.|[^\"])\'/ ; \
		comment : /;[^\\r\\n]*/ ; \
		symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&\\.]+/ ; \
		sexpr : '(' <expr>* ')' ; \
		qexpr : '{' <expr>* '}' ; \
		line : /^/ <expr>* /$/ ; \
		", Expr, Data, Double, Integer, Byte, Comment, String, Character, Symbol, Sexpr, Qexpr, Line);


	//printf("%d", sizeof(dval));

	denv* e = denv_new();
	denv_add_builtins(e);

	if (argc == 1) {
		puts("DanicLisp Version 0.2.0");
		puts("Press Ctrl+c to Exit\n");

		while (running) {
			char* input = readline("DanicL -> ");
			add_history(input);

			mpc_result_t r;
			if (mpc_parse("<stdin>", input, Line, &r)) {
				//mpc_ast_print(r.output);
				// TODO: do Lexical Scoping here?
				dval* x = dval_eval(e, dval_read(r.output));
				dval_println(x);
				dval_del(x);
				mpc_ast_delete(r.output);
			}
			else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}

			free(input);
		}
	} else if (argc >= 2) { // TODO: Lexical Scoping?
		for (int i = 1; i < argc; i++) {
			dval* args = dval_add(dval_sexpr(), dval_string(argv[i]));
			dval* x = builtin_load(e, args);
			if (x->type == DVAL_ERR) {
				dval_println(x);
			}
			dval_del(x);
		}
	}

	denv_del(e);
	mpc_cleanup(12, Expr, Data, Double, Integer, Byte, Comment, String, Character, Symbol, Sexpr, Qexpr, Line);
	return 0;
}
