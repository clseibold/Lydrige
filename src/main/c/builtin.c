#include "../headers/builtin.h"

dval *denv_put(denv *e, char *k, dval *v, int constant) { // Doesn't delete v
	dval *t;
	dval *item = (dval *) Hashmap_get(e->map, bfromcstr(k));
	if (item != NULL) { // If already defined in hashtable
		if (item->constant == 0) { // If not constant (in hashtable)
			dval *deleted = (dval *) Hashmap_delete(e->map, bfromcstr(k));
			dval_del(deleted); // Note that `deleted` is the same as `item`!
			item = NULL;
			t = dval_copy(v); // Copy value into t
			t->constant = constant; // set constant
			Hashmap_set(e->map, bfromcstr(k), t); // TODO: Check for errors!
			return(dval_int(0)); // TODO: Return something else?
		} else { // TODO
			return(dval_error("Cannot edit '%s'. It is a constant.", k));
		}
	} else { // Not in hashtable yet!
		//e->count++; // TODO: Doesn't hashmap have a count?
		t = dval_copy(v); // Copy value into t TODO: Possible Memory Leak - 't' never gets deleted/freed
		t->constant = constant; // set constant
		Hashmap_set(e->map, bfromcstr(k), t);
		return(dval_int(0)); // TODO: Return something else?
	}
}

dval *denv_get(denv *e, char *k) { // Copies the value gotten from hashmap, so it must be freed after calling this function!
	dval *v;
	if ((v = (dval *) Hashmap_get(e->map, bfromcstr(k)))) {
        dval *result = dval_copy(v); // TODO: Possible Memory Leak - should d get deleted? Not until end of program (when destroying hashmap)
		return(result);
	}

	if (e->parent) {
		return(denv_get(e->parent, k));
	}

	return(dval_error("Unbound symbol '%s'.", k));
}

dval *builtin_add(denv *e, dval *args, unsigned int argc) {
	if (argc == 0) {
		return(dval_error("Function '+' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
	}
	// Check if any are doubles. Set isDouble to true is so.
	int isDouble = false;
	for (int i = 0; i < argc; i++) {
		if (args[i].type == DVAL_LIST) { // DEBUG/DEV
			//print all of the elements
			builtin_print(e, args[i].elements, args[i].count);
		}
		if (!(args[i].type == DVAL_DOUBLE || args[i].type == DVAL_INT)) {
			return(dval_error("Function '+' may only be passed values of type int or double."));
		}
		if (args[i].type == DVAL_DOUBLE) {
			isDouble = true;
		}
	}
	if (isDouble) {
		double accumulator;
		if (args[0].type == DVAL_DOUBLE) {
			accumulator = args[0].doub;
		} else if (args[0].type == DVAL_INT) {
			accumulator = (double) args[0].integer;
		}
		for (int i = 1; i < argc; i++) {
			if (args[i].type == DVAL_INT) {
				accumulator += (double) args[i].integer;
			} else if (args[i].type == DVAL_DOUBLE) {
				accumulator += args[i].doub;
			}
		}
		return(dval_double(accumulator));
	} else {
		int accumulator = args[0].integer;
		for (int i = 1; i < argc; i++) {
			accumulator += args[i].integer;
		}
		return(dval_int(accumulator));
	}
}

dval *builtin_subtract(denv *e, dval *args, unsigned int argc) {
	if (argc == 0) {
		return(dval_error("Function '+' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
	}
	// Check if any are doubles. Set isDouble to true is so.
	int isDouble = false;
	for (int i = 0; i < argc; i++) {
		if (!(args[i].type == DVAL_DOUBLE || args[i].type == DVAL_INT)) {
			return(dval_error("Function '-' may only be passed values of type int or double."));
		}
		if (args[i].type == DVAL_DOUBLE) {
			isDouble = true;
		}
	}
	if (isDouble) {
		double accumulator;
		if (args[0].type == DVAL_DOUBLE) {
			accumulator = args[0].doub;
		} else if (args[0].type == DVAL_INT) {
			accumulator = (double) args[0].integer;
		}
		for (int i = 1; i < argc; i++) {
			if (args[i].type == DVAL_INT) {
				accumulator -= (double) args[i].integer;
			} else if (args[i].type == DVAL_DOUBLE) {
				accumulator -= args[i].doub;
			}
		}
		return(dval_double(accumulator));
	} else {
		int accumulator = args[0].integer;
		for (int i = 1; i < argc; i++) {
			accumulator -= args[i].integer;
		}
		return(dval_int(accumulator));
	}
}

dval *builtin_multiply(denv *e, dval *args, unsigned int argc) {
	if (argc == 0) {
		return(dval_error("Function '+' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
	}
	// Check if any are doubles. Set isDouble to true is so.
	int isDouble = false;
	for (int i = 0; i < argc; i++) {
		if (!(args[i].type == DVAL_DOUBLE || args[i].type == DVAL_INT)) {
			return(dval_error("Function '*' may only be passed values of type int or double."));
		}
		if (args[i].type == DVAL_DOUBLE) {
			isDouble = true;
		}
	}
	if (isDouble) {
		double accumulator;
		if (args[0].type == DVAL_DOUBLE) {
			accumulator = args[0].doub;
		} else if (args[0].type == DVAL_INT) {
			accumulator = (double) args[0].integer;
		}
		for (int i = 1; i < argc; i++) {
			if (args[i].type == DVAL_INT) {
				accumulator *= (double) args[i].integer;
			} else if (args[i].type == DVAL_DOUBLE) {
				accumulator *= args[i].doub;
			}
		}
		return(dval_double(accumulator));
	} else {
		int accumulator = args[0].integer;
		for (int i = 1; i < argc; i++) {
			accumulator *= args[i].integer;
		}
		return(dval_int(accumulator));
	}
}

dval *builtin_divide(denv *e, dval *args, unsigned int argc) {
	if (argc == 0) {
		return(dval_error("Function '/' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
	}
	// Check if any are doubles. Set isDouble to true is so.
	int isDouble = false;
	for (int i = 0; i < argc; i++) {
		if (args[i].type == DVAL_DOUBLE) {
			if (args[i].doub == 0.0 && i != 0) {
				return(dval_error("Function '/' - Cannot divide by zero."));
			}
			isDouble = true;
		} else if (args[i].type == DVAL_INT) {
			if (args[i].integer == 0 && i != 0) {
				return(dval_error("Function '/' - Cannot divide by zero."));
			}
		} else {
			return(dval_error("Function '/' may only be passed values of type int or double."));
		}
	}
	if (isDouble) {
		double accumulator;
		if (args[0].type == DVAL_DOUBLE) {
			accumulator = args[0].doub;
		} else if (args[0].type == DVAL_INT) {
			accumulator = (double) args[0].integer;
		}
		for (int i = 1; i < argc; i++) {
			if (args[i].type == DVAL_INT) {
				accumulator /= (double) args[i].integer;
			} else if (args[i].type == DVAL_DOUBLE) {
				accumulator /= args[i].doub;
			}
		}
		return(dval_double(accumulator));
	} else {
		int accumulator = args[0].integer;
		for (int i = 1; i < argc; i++) {
			accumulator /= args[i].integer;
		}
		return(dval_int(accumulator));
	}
}

dval *builtin_mod(denv *e, dval *args, unsigned int argc) {
	if (argc == 0) {
		return(dval_error("Function '%' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
	}
	// Check if any are doubles. Set isDouble to true is so.
	for (int i = 0; i < argc; i++) {
		if (args[i].type == DVAL_INT) {
			if (args[i].integer == 0 && i != 0) {
				return(dval_error("Function 'mod' - Cannot divide by zero."));
			}
		} else {
			return(dval_error("Function 'mod' may only be passed values of type int."));
		}
	}
	int accumulator = args[0].integer;
	for (int i = 1; i < argc; i++) {
		accumulator %= args[i].integer;
	}
	return(dval_int(accumulator));
}

dval *builtin_succ(denv *e, dval *args, unsigned int argc) {
	if (argc > 1) {
		return(dval_error("Function 'succ' may only be passed 1 argument."));
	}

	switch (args[0].type) {
		case DVAL_INT:
			return(dval_int(args[0].integer + 1));
		case DVAL_DOUBLE:
			return(dval_double(args[0].doub + 1));
		default:
			return(dval_error("Function 'succ' must be given values of type int or double."));
	}
}

dval *builtin_list(denv *e, dval *args, unsigned int argc) {
	if (argc == 0) {
		return(dval_error("Function 'list' must be passed 1 or more arguments."));
	}

	dval *largs = calloc(argc, sizeof(dval));
	memcpy(largs, args, sizeof(dval) * argc); // TODO: IFFY
	return(dval_list(largs, argc));
}

dval *builtin_len(denv *a, dval *args, unsigned int argc) {
	if (argc > 1 || argc == 0) {
		return(dval_error("Function 'len' must be passed only 1 argument."));
	}
	if (args[0].type != DVAL_LIST) {
		return(dval_error("Function 'len' must be passed a list."));
	}

	return(dval_int(args[0].count));
}

internal bool print_elem(dval arg) {
	switch (arg.type) {
		case DVAL_INT:
			printf("%d", arg.integer);
			return true;
		case DVAL_DOUBLE:
			printf("%f", arg.doub);
			return true;
		case DVAL_FUNC:
			printf("(Func)");
			return true;
		case DVAL_LIST:
			printf("[");
			for (int i = 0; i < arg.count; i++) {
				print_elem(arg.elements[i]);
				if (i != arg.count - 1) {
					printf(", ");
				}
			}
			printf("]");
			return true;
		default:
			return false;
	}
}

dval *builtin_print(denv *e, dval *args, unsigned int argc) {
	for (int i = 0; i < argc; i++) {
		if (!print_elem(args[i])) {
			return(dval_error("Cannot print value of type Unknown or Any!"));
		}
		printf(" ");
	}
	printf("\n");
	return(dval_int(1));
}

internal void denv_add_builtin(denv *e, char *name, dbuiltin func) {
	dval *v = dval_func(func, 1);
	dval_del(denv_put(e, name, v, v->constant)); // v is coppied (but not deleted in denv_put function)
	dval_del(v);
}

void denv_add_builtins(denv *e) {
	denv_add_builtin(e, "+", builtin_add);
	denv_add_builtin(e, "-", builtin_subtract);
	denv_add_builtin(e, "*", builtin_multiply);
	denv_add_builtin(e, "/", builtin_divide);
	denv_add_builtin(e, "mod", builtin_mod);
	denv_add_builtin(e, "succ", builtin_succ);

	denv_add_builtin(e, "list", builtin_list);
	denv_add_builtin(e, "len", builtin_len);

	denv_add_builtin(e, "print", builtin_print);
}