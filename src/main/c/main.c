#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = (char *) malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

#else
#include "../../linenoise/headers/linenoise.h"
#endif

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

typedef struct dval_or_darray {
	bool isArray;
	dval *result;
} dval_or_darray;

internal dval *read_eval_expr(denv *e, mpc_ast_t *t);

// Returns int of amount of args
internal int arg_amt(mpc_ast_t *t) {
	unsigned int argc = 0;
	for (unsigned int i = 0; i < t->children_num; i++) {
		if (strcmp(t->children[i]->contents, "(") == 0) continue;
		else if (strcmp(t->children[i]->contents, ")") == 0) continue;
		else if (strcmp(t->children[i]->contents, "'(") == 0) continue;
		else if (strcmp(t->children[i]->contents, "[") == 0) continue;
		else if (strcmp(t->children[i]->contents, "]") == 0) continue;
		else if (strcmp(t->children[i]->contents, ",") == 0) continue;
		else if (strcmp(t->children[i]->contents, ";") == 0) continue;
		else if (strcmp(t->children[i]->tag, "regex") == 0) continue;
		if (strstr(t->children[i]->tag, "ident") && !strstr(t->children[i]->tag, "value")) continue;
		argc++;
	}
	return argc;
}

// Returns array of dvals
internal dval_or_darray eval_args(int argc, mpc_ast_t *t, char **ident, denv *e) {
	dval *args = (dval*) calloc(argc, sizeof(dval));
	// Check for NULL, and return an error
	if (args == NULL) {
		return((dval_or_darray) { false, dval_error("Unable to allocate memory for arguments.") });
	}
	unsigned int currentArgPos = 0;
	for (unsigned int i = 0; i < t->children_num; i++) {
		if (strcmp(t->children[i]->contents, "(") == 0) continue;
		else if (strcmp(t->children[i]->contents, ")") == 0) continue;
		else if (strcmp(t->children[i]->contents, "'(") == 0) continue;
		else if (strcmp(t->children[i]->contents, "[") == 0) continue;
		else if (strcmp(t->children[i]->contents, "]") == 0) continue;
		else if (strcmp(t->children[i]->contents, ",") == 0) continue;
		else if (strcmp(t->children[i]->contents, ";") == 0) continue;
		else if (strcmp(t->children[i]->tag, "regex") == 0) continue;
		if (strstr(t->children[i]->tag, "ident") && !strstr(t->children[i]->tag, "value") && ident != NULL) {
			*ident = t->children[i]->contents;
			continue;
		}

		if (strstr(t->children[i]->tag, "expression")) {
			dval *d = read_eval_expr(e, t->children[i]);
			args[currentArgPos] = *d; // TODO: This gets coppied over. Is there a better way?
			if (d->type == DVAL_ERROR) {
				free(args);
				return((dval_or_darray) { false, d });
			} else {
				dval_del(d);
			}
			currentArgPos++;
		} else if (strstr(t->children[i]->tag, "list")) {
			unsigned int largc = arg_amt(t->children[i]);
			dval_or_darray elements = eval_args(largc, t->children[i], NULL, e);
			if (!elements.isArray) { // Error
				return((dval_or_darray) { false, elements.result });
			}
			args[currentArgPos] = (dval) { DVAL_LIST, 0, { .elements = elements.result }, largc };
			currentArgPos++;
		} else if (strstr(t->children[i]->tag, "statement")) {
			free(args);
			return((dval_or_darray) { false, read_eval_expr(e, t->children[i])}); // TODO(BUG): This will result in lines returning only its first statement's value
		} else if (strstr(t->children[i]->tag, "command")) { // REPL Only
			if (strcmp(t->children[i]->children[1]->contents, "exit") == 0) {
				running = false;
				free(args);
				return((dval_or_darray) { false, dval_error("Program Exited with Result: 1\n (User Interruption)\n") });
			} else if (strcmp(t->children[i]->children[1]->contents, "version") == 0) { // TODO: Make global version string
				printf(" Lydrige Version v0.6.0a\n");
				free(args);
				return((dval_or_darray) { false, dval_int(1) });
			} else if (strcmp(t->children[i]->children[1]->contents, "builtins") == 0) {
				printf(" basic operators (+, -, *, /, mod)\n succ - returns succession of given number (num + 1)\n list - returns list with given args as its elements\n print - prints out arguments\n len - returns length of list as an integer\n");
				free(args);
				return((dval_or_darray) { false, dval_int(1) });
			} else {
				free(args);
				return((dval_or_darray) { false, dval_error("Command doesn't exist.") });
			}
		} else if (strstr(t->children[i]->tag, "value")) {
			if (strstr(t->children[i]->tag, "ident")) {
				// TODO(FUTURE): Handle unary operators here
				// Evaluate identifier here, and add result to args
				dval *v = denv_get(e, t->children[i]->contents);
				if (v->type == DVAL_ERROR) {
					free(args);
					return((dval_or_darray) { false, v });
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
				char *substring;
				int substrlen = strlen(t->children[i]->contents) - 1;
				substring = malloc(substrlen * sizeof(char));
				memcpy(substring, &t->children[i]->contents[1], substrlen);
				substring[substrlen-1] = '\0';
				args[currentArgPos] = (dval) { DVAL_STRING, 0, {.str=substring}, 30 };
			} else if (strstr(t->children[i]->tag, "qexpr")) {
				free(args);
				return((dval_or_darray) { false, dval_error("Qexpressions are not completely implemented yet!") });
			}
			currentArgPos++;
		} else {
			free(args);
			return((dval_or_darray) { false, dval_error("[Interpreter] A value type was added to the parser but its evaluation is not handled.") });
		}
	}
	return((dval_or_darray) { true, args });
}

internal dval *read_eval_expr(denv *e, mpc_ast_t *t) {
	char *ident = ""; // TODO(Future): Eventually allow lambdas for function calls (also evaluate identifiers to be builtin functions or lambdas)

	unsigned int argc = arg_amt(t);
	dval_or_darray args = eval_args(argc, t, &ident, e);
	if (!args.isArray) {
		return(args.result);
	}
	
	if (strcmp(t->tag, ">") == 0) {
		free(args.result);
		return(NULL);
	}

	// Function/Lambda call
	dval *func = denv_get(e, ident);
	if (func->type == DVAL_FUNC) {
		dval *v = func->func(e, args.result, argc);
		free(args.result);
		free(func);
		return(v);
	} else if (func->type == DVAL_ERROR) {
		free(args.result);
		return(func);
	} else {
		free(args.result);
		free(func);
		return(dval_int(0));
	}
}

int main(int argc, char** argv) { // TODO: Possible memory leak from not calling bdestroy for all bstrings!
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
	Qexpression = mpc_new("qexpr");

	mpca_lang(MPCA_LANG_DEFAULT,
		"line : /^/ <command> /$/ | /^/ <statement>* /$/ ;"
		"command : ':' <ident> ;"
		"statement : <ident> <value>* ';' ;"
		"expression : '(' <ident> <value>* ')' ;"
		"value : <double> | <integer> | <character> | <string> | <expression> | <ident> | <list> | <qexpr> ;"
		"double : /-?[0-9]+\\.[0-9]+/ ;"
		"integer : /-?[0-9]+/ ;"
		"character : /\'(\\\\.|[^\"])\'/ ;"
		"string : /\"(\\\\.|[^\"])*\"/ ;"
		"ident : /[a-zA-Z0-9_\\-*\\/\\\\=<>!^%]+/ | '&' | '+' ;"
		"list : '[' (<value> (',' <value>)*)? ']' ;"
		"qexpr : \"'(\" <ident> <value>* ')' ;",
		Line, Command, Statement, Expression, Value, Double, Integer, Character, String, Identifier, List, Qexpression);

	if (argc == 1) {
		puts("Lydrige REPL - v0.6.0a");
		puts("Type ':exit' to Exit the REPL\n");

		denv *e = denv_new();
		denv_add_builtins(e);

#ifndef _WIN32
		linenoiseSetMultiLine(1);
		linenoiseHistorySetMaxLen(20);
#endif

		while (running) {
#ifdef _WIN32
			char *input = readline("Lydrige> ");
#else
			char *input = linenoise("Lydrige> ");
			linenoiseHistoryAdd(input);
#endif

			mpc_result_t r;
			if (mpc_parse("<stdin>", input, Line, &r)) {
				//mpc_ast_print((mpc_ast_t*) r.output); puts("");
				dval *result = read_eval_expr(e, (mpc_ast_t *) r.output);
				if (result->type == DVAL_ERROR) {
					printf("Error: %s\n", result->str);
				} else {
					bool known = print_elem(*result, false);
					printf("\n");
					if (!known) {
						printf("Error: Cannot print value of type Unknown or Any!");
					}
				}
				dval_del(result);
				mpc_ast_delete((mpc_ast_t *) r.output);
			} else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}
#ifdef _WIN32
			free(input);
#else
			linenoiseFree(input);
#endif
		}

		denv_del(e);
	} else if (argc == 2) {
		// Read file and evaluate each line here!
	}

	mpc_cleanup(12, Line, Command, Statement, Expression, Value, Double, Integer, Character, String, Identifier, List, Qexpression);
	return(0);
}
