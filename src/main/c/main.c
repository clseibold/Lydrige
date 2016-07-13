#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include "../../linenoise/headers/linenoise.h"

#include "../../mpc/headers/mpc.h"
#include "../headers/structure.h"
#include "../headers/builtin.h"

#define COL_RED "\x1b[31m"
#define COL_GREEN "\x1b[32m"
#define COL_YELLOW "\x1b[33m"
#define COL_BLUE "\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN "\x1b[36m"
#define COL_RESET "\x1b[0m"

internal dval *read_eval_expr(denv *e, mpc_ast_t* t) {
	// For loop to determine amount of args used
	unsigned int argc = 0;
	for (unsigned int i = 0; i < t->children_num; i++) {
		if (strcmp(t->children[i]->contents, "(") == 0) continue;
		else if (strcmp(t->children[i]->contents, ")") == 0) continue;
		else if (strcmp(t->children[i]->contents, "[") == 0) continue;
		else if (strcmp(t->children[i]->contents, "]") == 0) continue;
		else if (strcmp(t->children[i]->contents, ",") == 0) continue;
		else if (strcmp(t->children[i]->contents, ";") == 0) continue;
		else if (strcmp(t->children[i]->tag, "regex") == 0) continue;
		if (strstr(t->children[i]->tag, "ident") && !strstr(t->children[i]->tag, "value")) continue;
		argc++;
	}

	dval *args = (dval*) calloc(argc, sizeof(dval));
	// Check for NULL, and return an error
	if (args == NULL) {
		return(dval_error("Unable to allocate memory for arguments."));
	}
	char* ident; // TODO(Future): Eventually allow lambdas for function calls (also evaluate identifiers to be builtin functions or lambdas)
	dval *result;

	unsigned int currentArgPos = 0;
	for (unsigned int i = 0; i < t->children_num; i++) {
		if (strcmp(t->children[i]->contents, "(") == 0) continue;
		else if (strcmp(t->children[i]->contents, ")") == 0) continue;
		else if (strcmp(t->children[i]->contents, "[") == 0) continue;
		else if (strcmp(t->children[i]->contents, "]") == 0) continue;
		else if (strcmp(t->children[i]->contents, ",") == 0) continue;
		else if (strcmp(t->children[i]->contents, ";") == 0) continue;
		else if (strcmp(t->children[i]->tag, "regex") == 0) continue;
		if (strstr(t->children[i]->tag, "ident") && !strstr(t->children[i]->tag, "value")) {
			ident = t->children[i]->contents;
			continue;
		}

		if (strstr(t->children[i]->tag, "expression")) {
			dval *d = read_eval_expr(e, t->children[i]);
			args[currentArgPos] = *d; // TODO: This gets coppied over. Is there a better way?
			if (d->type == DVAL_ERROR) {
				result = d;
				free(args);
				return(result);
			} else {
				dval_del(d);
			}
			currentArgPos++;
		} else if (strstr(t->children[i]->tag, "list")) {
			// TODO(CLEANUP): This is essentially just a copy of code from the outer loop!
			// For loop to get element count
			unsigned int lcount = 0;
			for (int ii = 0; ii < t->children[i]->children_num; ii++) {
				if (strcmp(t->children[i]->children[ii]->contents, "(") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, ")") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, "[") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, "]") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, ",") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, ";") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->tag, "regex") == 0) continue;
				lcount++;
			}

			dval *elements = calloc(lcount, sizeof(dval));

			// Add the elements
			unsigned int lcurrentArgPos = 0;
			for (int ii = 0; ii < t->children[i]->children_num; ii++) {
				if (strcmp(t->children[i]->children[ii]->contents, "[") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, "]") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, ",") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->contents, ";") == 0) continue;
				else if (strcmp(t->children[i]->children[ii]->tag, "regex") == 0) continue;
				if (strstr(t->children[i]->children[ii]->tag, "expression")) {
					dval *d = read_eval_expr(e, t->children[i]->children[ii]);
					elements[lcurrentArgPos] = *d;
					if (d->type == DVAL_ERROR) {
						result = d;
						free(args);
						free(elements);
						return(result);
					} else {
						dval_del(d);
					}
				} else if (strstr(t->children[i]->children[ii]->tag, "value")) {
					if (strstr(t->children[i]->children[ii]->tag, "int")) {
						elements[lcurrentArgPos] = (dval) { DVAL_INT, 0, {strtol(t->children[i]->children[ii]->contents, NULL, 10)} };
					} else if (strstr(t->children[i]->children[ii]->tag, "double")) {
						elements[lcurrentArgPos] = (dval) { DVAL_DOUBLE, 0, {.doub=strtod(t->children[i]->children[ii]->contents, NULL)} };
					} else if (strstr(t->children[i]->children[ii]->tag, "character")) {
						elements[lcurrentArgPos] = (dval) { DVAL_CHARACTER, 0, {.character=t->children[i]->children[ii]->contents[1]} };
					} else if (strstr(t->children[i]->children[ii]->tag, "string")) {
						free(args); free(elements);
						return(dval_error("Strings are not completely implemented yet!"));
					} else if (strstr(t->children[i]->children[ii]->tag, "ident")) {
						dval *v = denv_get(e, t->children[i]->children[ii]->contents);
						if (v->type == DVAL_ERROR) {
							result = v;
							free(args);
							free(elements);
							return(result);
						} else {
							elements[lcurrentArgPos] = *v; // TODO(NOTE): This is coppied
						}
					} // TODO: else
				} else {
					// TODO: Error?
					elements[lcurrentArgPos] = (dval) { DVAL_INT, 0, {0} };
				}
				lcurrentArgPos++;
			}

			args[currentArgPos] = (dval) { DVAL_LIST, 0, { .elements = elements }, lcount };
			currentArgPos++;
		} else if (strstr(t->children[i]->tag, "statement")) {
			result = read_eval_expr(e, t->children[i]); // TODO(BUG): This will result in lines returning only its last statement's value
		} else if (strstr(t->children[i]->tag, "command")) { // REPL Only
			if (strcmp(t->children[i]->children[1]->contents, "exit") == 0) {
				running = false;
				result = dval_error("Program Exited with Result: 1\n (User Interruption)\n");
				free(args);
				return(result);
			} else if (strcmp(t->children[i]->children[1]->contents, "version") == 0) { // TODO: Make global version string
				printf(" Lydrige Version v0.6.0a\n");
				result = dval_int(1);
			} else if (strcmp(t->children[i]->children[1]->contents, "builtins") == 0) {
				printf(" basic operators (+, -, *, /, mod)\n succ - returns succession of given number (num + 1)\n list - returns list with given args as its elements\n print - prints out arguments\n len - returns length of list as an integer\n");
				result = dval_int(1);
			} else {
				result = dval_error("Command doesn't exist.");
				free(args);
				return(result);
			}
		} else if (strstr(t->children[i]->tag, "value")) {
			if (strstr(t->children[i]->tag, "ident")) {
				// TODO(FUTURE): Handle unary operators here
				// Evaluate identifier here, and add result to args
				dval *v = denv_get(e, t->children[i]->contents);
				if (v->type == DVAL_ERROR) {
					result = v;
					free(args);
					return(result);
				} else {
					args[currentArgPos] = *v; // TODO: This gets copied
				}
			} else if (strstr(t->children[i]->tag, "integer")) {
				args[currentArgPos] = (dval) { DVAL_INT, 0, {strtol(t->children[i]->contents, NULL, 10)} };
			} else if (strstr(t->children[i]->tag, "double")) {
				args[currentArgPos] = (dval) { DVAL_DOUBLE, 0, {.doub=strtod(t->children[i]->contents, NULL)} };
			} else if (strstr(t->children[i]->tag, "character")) {
				args[currentArgPos] = (dval) { DVAL_CHARACTER, 0, {.character=t->children[i]->contents[1]} };
			} else if (strstr(t->children[i]->tag, "string")) {
				free(args);
				return(dval_error("String are not completely implemented yet!"));
			} else { // TODO: Shouldn't I just be returning the error (instead of setting it to an argument)?
				args[currentArgPos] = (dval) { DVAL_ERROR, 0, {.str = "(Interpreter error) A value type was added to the parser, but it's evaluation is not handled."} };
			}
			currentArgPos++;
		} else {
			args[currentArgPos] = (dval) { DVAL_ANY, 0, {0} };
			currentArgPos++;
		}
	}

	if (strcmp(t->tag, ">") == 0) {
		free(args);
		return(result);
	}

	// Function/Lambda call
	dval *func = denv_get(e, ident);
	if (func->type == DVAL_FUNC) {
		dval *v = func->func(e, args, argc);
		free(args);
		free(func);
		return(v);
	} else if (func->type == DVAL_ERROR) {
		free(args);
		return(func);
	} else {
		free(args);
		free(func);
		return(dval_int(0));
	}
}

int main(int argc, char** argv) { // TODO: Memory leak from not calling bdestroy for all bstrings!
	Line = mpc_new("line");
	Command = mpc_new("command"); // REPL-Only commands
	Statement = mpc_new("statement");
	Expression = mpc_new("expression");
	Value = mpc_new("value");
	Integer = mpc_new("integer");
	Double = mpc_new("double");
	Character = mpc_new("character");
	String = mpc_new("string");
	Identifier = mpc_new("ident");
	List = mpc_new("list");

	mpca_lang(MPCA_LANG_DEFAULT,
		" \
		line : /^/ <command> /$/ | /^/ <expression>* /$/ | /^/ <statement>* /$/ ;\
		command : ':' <ident> ;\
		statement : <ident> <value>* ';' ;\
		expression : '(' <ident> <value>* ')' ;\
		value : <double> | <integer> | <character> | <string> | <expression> | <ident> | <list>;\
		double : /-?[0-9]+\\.[0-9]+/ ;\
		integer : /-?[0-9]+/ ;\
		character : /\'(\\\\.|[^\"])\'/ ;\
		string : /\"(\\\\.|[^\"])*\"/ ;\
		ident : /[a-zA-Z0-9_\\-*\\/\\\\=<>!^%]+/ | '&' | '+' ;\
		list : '[' (<value> (',' <value>)*)? ']' ;\
		", Line, Command, Statement, Expression, Value, Double, Integer, Character, String, Identifier, List);

	if (argc == 1) {
		puts("Lydrige REPL - v0.6.0a");
		puts("Type ':exit' to Exit the REPL\n");

		denv *e = denv_new();
		denv_add_builtins(e);

		linenoiseSetMultiLine(1);
		linenoiseHistorySetMaxLen(20);
		while (running) {
			char* input = linenoise("Lydrige> ");
			linenoiseHistoryAdd(input);

			mpc_result_t r;
			if (mpc_parse("<stdin>", input, Line, &r)) {
				// mpc_ast_print((mpc_ast_t*) r.output); puts("");
				dval *result = read_eval_expr(e, (mpc_ast_t *) r.output);
				print_elem(*result, false); // TODO(BUG): 'print' function should return '1' directly after being ran, but '1' is only printed after entering another expression in REPL?!?!
				dval_del(result);
				mpc_ast_delete((mpc_ast_t *) r.output);
			} else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}
			linenoiseFree(input);
		}

		denv_del(e);
	}

	mpc_cleanup(11, Line, Command, Statement, Expression, Value, Double, Integer, Character, String, Identifier, List);
	return(0);
}
