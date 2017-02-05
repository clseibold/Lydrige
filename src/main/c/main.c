#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

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

#include "../../mpc/headers/mpc.h"
#include "../headers/structures.h"
#include "../headers/reading.h"
#include "../headers/printing.h"
#include "../headers/builtin.h"

int main(int argc, char** argv) {
	Expr = mpc_new("expr");
	Data = mpc_new("data");
	Lambda = mpc_new("lambda");
	Double = mpc_new("double");
	Integer = mpc_new("integer");
	LByte = mpc_new("byte");
	Range = mpc_new("range");
	Comment = mpc_new("comment");
	String = mpc_new("string");
	Character = mpc_new("character");
	Symbol = mpc_new("symbol");
	Note = mpc_new("note");
	List = mpc_new("list");
	Sexpr = mpc_new("sexpr");
	Qexpr = mpc_new("qexpr");
	Index = mpc_new("index");
	Statement = mpc_new("statement");
	Line = mpc_new("line");

	mpca_lang(MPCA_LANG_DEFAULT,
		" \
		expr		: <data> | <symbol> | <sexpr> | <qexpr> | <list> | <comment> ; \
		data		: <lambda> | <index> | <byte> | <double> | <range> | <integer> | <string> | <character> | <note> ; \
		lambda		: <qexpr> \"->\" <qexpr> | <qexpr> \"->\" <list> ; \
		index 		: <qexpr> '[' <integer> ']' | <symbol> '[' <integer> ']' ; \
		byte		: /0x[0-9a-fA-F][0-9a-fA-F]/ ; \
		double		: /-?[0-9]+\\.[0-9]+/ ; \
		range		: <integer> \"..\" <integer> ; \
		integer		: /-?[0-9]+/ ; \
		string		: /\"(\\\\.|[^\"])*\"/ ; \
		character	: /\'(\\\\.|[^\"])\'/ ; \
		note		: ':' <symbol> | ':' '(' <symbol>+ ')' ; \
		symbol		: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!\\.^%]+/ | '&' ; \
		sexpr		: '(' <expr>* ')' ; \
		qexpr		: '{' <expr>* '}' ; \
		list		: '[' <expr>* ']' ; \
		comment		: /#[^\\r\\n]*/ ; \
		statement	: <expr>+ ';' | <comment> ; \
		line		: /^/ <expr>* /$/ | /^/ <statement>* /$/ ; \
		", Expr, Data, Lambda, Double, Integer, LByte, Range, Comment, String, Character, Symbol, Note, List, Sexpr, Qexpr, Index, Statement, Line);

	denv* e = denv_new();
	denv_add_builtins(e);

	denv_def(e, dval_string("argc"), dval_int(argc - 2), 1);
	dval* args = dval_qexpr();
	if (argc > 2) {
		for (unsigned int i = 2; i < argc; i++) {
			dval* str = dval_string(argv[i]);
			dval_add(args, str);
		}
	}
	denv_def(e, dval_string("args"), args, 1);

	if (argc == 1) {
		puts("Lydrige REPL - Version 0.5.0");
		puts("Type 'exit' to Exit the REPL\n");
		// Load prelude here!
		dval* pargs = dval_add(dval_sexpr(), dval_string("./examples/newstdlib.lydg"));
		dval* prelude = builtin_load(e, pargs);
		if (prelude->type == DVAL_ERR) {
			dval_println(prelude);
			dval_del(prelude);
		} else {
			while (running) {
				char* input = readline(COL_GREEN "Lydrige -> " COL_RESET);
				add_history(input);

				mpc_result_t r;
				if (mpc_parse("<stdin>", input, Line, &r)) {
					//mpc_ast_print((mpc_ast_t*)r.output);
					dval* x = dval_eval(e, dval_read(e, (mpc_ast_t*)r.output));
					dval_println(x);
					dval_del(x);
					mpc_ast_delete((mpc_ast_t*)r.output);
				}
				else { // TODO: Better error messages here! (can start with not having ';' at end)
					mpc_err_print(r.error);
					mpc_err_delete(r.error);
				}

				free(input);
			}
			dval_del(prelude);
		}
	} else if (argc >= 2) {
		// Load prelude here!
		dval* pargs = dval_add(dval_sexpr(), dval_string("./examples/newstdlib.lydg"));
		dval* prelude = builtin_load(e, pargs);
		if (prelude->type == DVAL_ERR) {
			dval_println(prelude);
			dval_del(prelude);
		} else {
			/*for (int i = 1; i < argc; i++) {
				dval* args = dval_add(dval_sexpr(), dval_string(argv[i]));
				dval* x = builtin_load(e, args);
				if (x->type == DVAL_ERR) {
					dval_println(x);
					dval_del(x);
					break;
				}
				dval_del(x);
			}*/
			dval* arg = dval_add(dval_sexpr(), dval_string(argv[1]));
			dval* x = builtin_load(e, arg);
			if (x->type == DVAL_ERR) {
				dval_println(x);
			}
			dval_del(x);
			dval_del(prelude);
		}
	}

	denv_del(e);
	mpc_cleanup(18, Expr, Lambda, Data, Double, Integer, LByte, Range, Comment, String, Character, Symbol, Note, List, Sexpr, Qexpr, Index, Statement,  Line);
	return 0;
}
