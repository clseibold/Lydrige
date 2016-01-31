#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

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

#include "../headers/hashtable.h"
#include "../../mpc/headers/mpc.h"
#include "../headers/structures.h"
#include "../headers/reading.h"
#include "../headers/printing.h"
#include "../headers/builtin.h"

int main(int argc, char** argv) {
	Expr = mpc_new("expr");
	Data = mpc_new("data");
	Double = mpc_new("double");
	Integer = mpc_new("integer");
	Byte = mpc_new("byte");
	Range = mpc_new("range");
	Comment = mpc_new("comment");
	String = mpc_new("string");
	Character = mpc_new("character");
	Symbol = mpc_new("symbol");
	Note = mpc_new("note");
	List = mpc_new("list");
	SList = mpc_new("slist");
	Sexpr = mpc_new("sexpr");
	SSexpr = mpc_new("ssexpr");
	Qexpr = mpc_new("qexpr");
	SQexpr = mpc_new("sqexpr");
	Line = mpc_new("line");

	mpca_lang(MPCA_LANG_DEFAULT,
		" \
		expr		: <data> | <symbol> | <ssexpr> | <sexpr> | <sqexpr> | <qexpr> | <slist> | <list> | <comment> ; \
		data		: <byte> | <double> | <range> | <integer> | <string> | <character> | <note> ; \
		double		: /-?[0-9]+\\.[0-9]+/ ; \
		integer		: /-?[0-9]+/ ; \
		byte		: /0x[0-9a-fA-F][0-9a-fA-F]/ ; \
		range		: <integer> \"..\" <integer> ; \
		string		: /\"(\\\\.|[^\"])*\"/ ; \
		character	: /\'(\\\\.|[^\"])\'/ ; \
		comment		: /;[^\\r\\n]*/ ; \
		symbol		: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&\\.^]+/ ; \
		note		: ':' <symbol> ; \
		slist		: \"\\'[\" <expr>* ']' ; \
		list		: '[' <expr>* ']' ; \
		ssexpr		: \"\\'(\" <expr>* ')' ; \
		sexpr		: '(' <expr>* ')' ; \
		sqexpr		: \"\\'{\" <expr>* '}' ; \
		qexpr		: '{' <expr>* '}' ; \
		line		: /^/ <expr>* /$/ ; \
		", Expr, Data, Double, Integer, Byte, Range, Comment, String, Character, Symbol, Note, List, SList, SSexpr, Sexpr, SQexpr, Qexpr, Line);

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
				//mpc_ast_print((mpc_ast_t*)r.output);
				dval* x = dval_eval(e, dval_read((mpc_ast_t*)r.output));
				dval_println(x);
				dval_del(x);
				mpc_ast_delete((mpc_ast_t*)r.output);
			}
			else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}

			free(input);
		}
	} else if (argc >= 2) {
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
	mpc_cleanup(18, Expr, Data, Double, Integer, Byte, Range, Comment, String, Character, Symbol, Note, List, SSexpr, SList, Sexpr, SQexpr, Qexpr, Line);
	return 0;
}
