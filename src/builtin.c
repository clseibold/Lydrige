/* Lydrige - A simple interpreted programming language inspired by Lisp.
 * Copyright (c) 2016, Christian Seibold
 * Under MIT License
 *
 * You can find latest source code at:
 * https://github.com/krixano/Lydrige
 *
 * -----------------------------------------------------------------------
 * MIT License
 *
 * Copyright (c) 2016 Christian Seibold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * -----------------------------------------------------------------------
 */

#include <builtin.h>
#include <colors.h>
#include <stdlib.h>
#include <math.h>

/*
* k - key
* v - value
* constant - not editable/modifiable
* You must delete v yourself, the function will not do this for you
*/
dval *denv_put(denv *e, char *k, dval *v, int constant) {
    dval *t;
    bstring k_bstr = bfromcstr(k);
    dval *item = (dval *) Hashmap_get(e->map, k_bstr);
    if (item != NULL) { // If already defined in hashtable
        if (item->constant == 0) { // If not constant (in hashtable)
            dval *deleted = (dval *) Hashmap_delete(e->map, k_bstr);
            dval_del(deleted); // Note that `deleted` is the same as `item`!
            item = NULL;
            t = dval_copy(v); // Copy value into t
            t->constant = constant; // set constant
            Hashmap_set(e->map, k_bstr, t); // TODO: Check for errors!
            return(denv_get(e, "true")); // TODO: Return something else?
        } else { // TODO
            bdestroy(k_bstr);
            return(dval_error("Cannot edit '%s'. It is a constant.", k));
        }
    } else { // Not in hashtable yet!
        //e->count++; // TODO: Doesn't hashmap have a count?
        t = dval_copy(v);
        t->constant = constant; // set constant
        Hashmap_set(e->map, k_bstr, t); // TODO: Check for errors!
        return(denv_get(e, "true")); // Return true
    }
}

dval *denv_get(denv *e, char *k) { // Copies the value gotten from hashmap, so it must be freed after calling this function!
    dval *v;
    bstring k_bstr = bfromcstr(k);
    if ((v = (dval *) Hashmap_get(e->map, k_bstr))) {
        dval *result = dval_copy(v); // TODO: Possible Memory Leak - should d get deleted? Not until end of program (when destroying hashmap)
        bdestroy(k_bstr);
        return(result);
    }
    bdestroy(k_bstr);
    
    if (e->parent) {
        return(denv_get(e->parent, k));
    }
    
    return(dval_error("Unbound symbol '%s'.", k));
}

dval *builtin_add(denv *e, dval *args, unsigned int argc) {
    /*if (argc == 0) {
        return(dval_error("Function '+' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
    }*/
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
        double accumulator = 0.0;
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
    /*if (argc == 0) {
        return(dval_error("Function '+' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
    }*/
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
    /*if (argc == 0) {
        return(dval_error("Function '+' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
    }*/
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
        double accumulator = 0.0;
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
    /*if (argc == 0) {
        return(dval_error("Function '/' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
    }*/
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
        double accumulator = 0.0;
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
    /*if (argc == 0) {
        return(dval_error("Function '%' given wrong amount of arguments. Got '%d', Expected at least 1.", argc));
    }*/
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

dval *builtin_power(denv *e, dval *args, unsigned int argc) {
    /*if (argc != 2) {
        return(dval_error("Function '^' given wrong amount of arguments. Got '%d', Expected 2.", argc));
    }*/

    if (argc == 1) {
        if (args[0].type == DVAL_INT) {
            return dval_int(args[0].integer);
        } else if (args[0].type == DVAL_DOUBLE) {
            return dval_double(args[0].doub);
        } else {
            return dval_error("Function '^' may only be passed values of type int or double.");
        }
    } else {
        if (args[0].type == DVAL_INT && args[1].type == DVAL_INT) {
            return dval_int(pow(args[0].integer, args[1].integer));
        } else if (args[0].type == DVAL_INT && args[1].type == DVAL_DOUBLE) {
            return dval_double(pow(args[0].integer, args[1].doub));
        } else if (args[0].type == DVAL_DOUBLE && args[1].type == DVAL_INT) {
            return dval_double(pow(args[0].doub, args[1].integer));
        } else if (args[0].type == DVAL_DOUBLE && args[1].type == DVAL_DOUBLE) {
            return dval_double(pow(args[0].doub, args[1].doub));
        } else {
            return dval_error("Function '^' may only be passed values of type int or double.");
        }
    }
}

dval *builtin_succ(denv *e, dval *args, unsigned int argc) {
    /*if (argc > 1) {
        return(dval_error("Function 'succ' may only be passed 1 argument."));
    }*/
    
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
    /*if (argc == 0) {
		//return(dval_error("Function 'list' must be passed 1 or more arguments."));
		return(dval_list((void *) 0, 0));
    }*/
    
    dval *largs = calloc(argc, sizeof(dval));
    memcpy(largs, args, sizeof(dval) * argc);
    return(dval_list(largs, argc));
}

dval *builtin_len(denv *a, dval *args, unsigned int argc) {
    /*if (argc > 1 || argc == 0) {
        return(dval_error("Function 'len' must be passed only 1 argument."));
    }
    if (args[0].type != DVAL_LIST) {
        return(dval_error("Function 'len' must be passed a list."));
    }*/
    
    return(dval_int(args[0].count));
}

dval *builtin_get(denv *a, dval *args, unsigned int argc) {
    /*if (argc != 2) {
        return(dval_error("Function 'get' must be passed 2 arguments."));
    }
    if (args[0].type != DVAL_INT) {
        return(dval_error("Function 'get' must be passed an integer for argument 1."));
    }
    if (args[1].type != DVAL_LIST) {
        return(dval_error("Function 'get' must be passed a list for argument 2."));
    }*/
    
    dval *arg = calloc(1, sizeof(dval));
    memcpy(arg, args[1].elements + args[0].integer, sizeof(dval) * 1); // Copy element at given index into arg
    return(arg);
}

dval *builtin_set(denv *a, dval *args, unsigned int argc) {
    /*if (argc > 3 || argc == 0) {
        return(dval_error("Function 'get' must be passed 3 arguments."));
    }
    if (args[0].type != DVAL_INT) {
        return(dval_error("Function 'get' must be passed an integer for argument 1."));
    }
    if (args[2].type != DVAL_LIST) {
        return(dval_error("Function 'get' must be passed a list for argument 3."));
    }*/
    
    dval *largs = calloc(args[2].count, sizeof(dval));
    memcpy(largs, args[2].elements, sizeof(dval) * (args[2].count)); // Copy all elements into arg
    largs[args[0].integer] = args[1];
    return(dval_list(largs, args[2].count));
}

dval *builtin_first(denv *a, dval *args, unsigned int argc) {
    /*if (argc > 1 || argc == 0) {
        return(dval_error("Function 'first' must be passed only 1 argument."));
    }
    if (args[0].type != DVAL_LIST) {
        return(dval_error("Function 'first' must be passed a list."));
    }*/
    
    dval *arg = calloc(1, sizeof(dval));
    memcpy(arg, args[0].elements, sizeof(dval) * 1); // Copy only the first element into arg
    return(arg);
}

dval *builtin_last(denv *a, dval *args, unsigned int argc) {
    /*if (argc > 1 || argc == 0) {
        return(dval_error("Function 'last' must be passed only 1 argument."));
    }
    if (args[0].type != DVAL_LIST) {
        return(dval_error("Function 'last' must be passed a list."));
    }*/
    
    dval *arg = calloc(1, sizeof(dval));
    memcpy(arg, args[0].elements+(args[0].count-1), sizeof(dval) * 1); // Copy only the very last element into arg
    return(arg);
}

dval *builtin_head(denv *a, dval *args, unsigned int argc) {
    /*if (argc > 1 || argc == 0) {
        return(dval_error("Function 'head' must be passed only 1 argument."));
    }
    if (args[0].type != DVAL_LIST) {
        return(dval_error("Function 'head' must be passed a list."));
    }*/
    
    unsigned int count = args[0].count - 1;
    dval *largs = calloc(count, sizeof(dval));
    memcpy(largs, args[0].elements, sizeof(dval) * (args[0].count - 1)); // Copy all but last element into largs
    return(dval_list(largs, count));
}

dval *builtin_tail(denv *a, dval *args, unsigned int argc) {
    /*if (argc > 1 || argc == 0) {
        return(dval_error("Function 'head' must be passed only 1 argument."));
    }
    if (args[0].type != DVAL_LIST) {
        return(dval_error("Function 'head' must be passed a list."));
    }*/
    
    unsigned int count = args[0].count - 1;
    dval *largs = calloc(count, sizeof(dval));
    memcpy(largs, args[0].elements + 1, sizeof(dval) * (args[0].count - 1)); // Copy all but first element into largs
    return(dval_list(largs, count));
}

dval *builtin_join(denv *a, dval *args, unsigned int argc) {
    /*if (argc <= 1 || argc > 2) {
        return(dval_error("Function 'join' must be passed only 2 arguments."));
    }
    if (args[0].type != DVAL_LIST || args[1].type != DVAL_LIST) {
        return(dval_error("Function 'join' must be passed lists."));
    }*/
    
    unsigned int largc = args[0].count + args[1].count;
    dval *largs = calloc(largc, sizeof(dval));
    memcpy(largs, args[0].elements, sizeof(dval) * args[0].count);
    memcpy(largs + args[0].count, args[1].elements, sizeof(dval) * args[1].count);
    dval *list = dval_list(largs, largc);
    return(list);
}

int check_types(dval *typeArg, dval *arg, dval **error) {
    switch (typeArg->typeValue) {
        case DVAL_ANY:
        {} break;
        
        case DVAL_INT:
        {
            if (arg->type != DVAL_INT) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_DOUBLE:
        {
            if (arg->type != DVAL_DOUBLE) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_CHARACTER:
        {
            if (arg->type != DVAL_CHARACTER) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_STRING:
        {
            if (arg->type != DVAL_STRING) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_FUNC:
        {
            if (arg->type != DVAL_FUNC) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_LIST:
        {
            if (arg->type != DVAL_LIST) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_TYPEVALUE:
        {
            if (arg->type != DVAL_TYPEVALUE) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        case DVAL_IDENT:
        {
            if (arg->type != DVAL_IDENT) {
                *error = dval_error("Value passed into function 'def' does not match given type '%s'", get_type_string(typeArg->typeValue));
                return 0;
            }
        } break;
        
        default:
        {
            *error = dval_error("Unknown type '%s'!", get_type_string(typeArg->typeValue));
            return 0;
        } break;
    }

    return 1; // TODO: Return true/false (using denv_get)?
}

dval *builtin_def(denv *a, dval *args, unsigned int argc) {
    /*if (argc != 3) {
        return(dval_error("Function 'def' must be passed only 3 arguments."));
    }
    if (args[0].type != DVAL_IDENT) {
        return(dval_error("Function 'def' must be passed quoted identifier for argument 1"));
    }
    if (args[1].type != DVAL_TYPEVALUE) {
        return dval_error("Function 'def' must be passed a type for argument 2.");
    }*/

    dval *error;
    if (!check_types(&args[1], &args[2], &error)) {
        return error;
    }
    
    dval *output = denv_put(a, args[0].str, dval_copy(&args[2]), false);
    if (output->type == DVAL_ERROR) {
        return output;
    } else {
        dval_del(output);
    }
    return denv_get(a, args[0].str);
}

dval *builtin_const(denv *a, dval *args, unsigned int argc) {
    /*if (argc != 3) {
        return(dval_error("Function 'const' must be passed only 3 arguments."));
    }
    if (args[0].type != DVAL_IDENT) {
        return(dval_error("Function 'const' must be passed quoted identifier for argument 1"));
    }
    if (args[1].type != DVAL_TYPEVALUE) {
        return dval_error("Function 'const' must be passed a type for argument 2.");
    }*/

    dval *error;
    if (!check_types(&args[1], &args[2], &error)) {
        return error;
    }
    
    dval *output = denv_put(a, args[0].str, dval_copy(&args[2]), true);
    if (output->type == DVAL_ERROR) {
        return output;
    } else {
        dval_del(output);
    }
    return denv_get(a, args[0].str);
}

dval *builtin_typeof(denv *a, dval *args, unsigned int argc) {
    /*if (argc < 1 || argc > 1) {
        return(dval_error("Function 'typeof' must be passed one argument only."));
    }*/
    
    dval *type = dval_type(args[0].type);
    return type;
}

dval *builtin_cast(denv *a, dval *args, unsigned int argc) {
    /*if (argc != 2) {
        return dval_error("Function 'cast' must be passed 2 arguments.");
    }
    if (args[0].type != DVAL_TYPEVALUE) {
        return dval_error("Function 'cast' must be passed a type for argument 1.");
    }*/

    if (args[0].typeValue == DVAL_INT && args[1].type == DVAL_DOUBLE) {
        return dval_int((int) args[1].doub);
    } else if (args[0].typeValue == DVAL_DOUBLE && args[1].type == DVAL_INT) {
        return dval_double((double) args[1].integer);
    } else {
        return dval_error("Function 'cast' cannot cast value to type '%s'.", get_type_string(args[0].typeValue));
    }
}

char *get_type_string(DVAL_TYPE type) {
    switch(type) {
        case DVAL_ANY:
        {
            return("any");
        } break;
        
        case DVAL_INT:
        {
            return("int");
        } break;
        
        case DVAL_DOUBLE:
        {
            return("double");
        } break;
        
        case DVAL_CHARACTER:
        {
            return("char");
        } break;
        
        case DVAL_STRING:
        {
            return("string");
        } break;
        
        case DVAL_FUNC:
        {
            return("func");
        } break;
        
        case DVAL_LIST:
        {
            return("array");
        } break;
        
        case DVAL_TYPEVALUE:
        {
            return("type");
        } break;
        
        case DVAL_IDENT:
        {
            return("qident"); // Change this name
        } break;
        
        default:
        {
            return("unknown_type");
        } break;
    }
}

dval *builtin_if(denv *a, dval *args, unsigned int argc) {
    /*if (argc != 3) {
        return dval_error("Function 'if' must be passed only 3 arguments.");
    }
    if (args[0].type != DVAL_INT) {
        return dval_error("Function 'if' must be passed an integer for argument 1.");
    }*/

    if (args[0].integer == denv_get(a, "true")->integer) {
        return dval_copy(&args[1]);
    } else {
        return dval_copy(&args[2]);
    }
}

// and - returns 0 (true) if all given integers/doubles are 0 (true). Returns 0 if any given integer/double is 0.
dval *builtin_and(denv *a, dval *args, unsigned int argc) {
    /*if (argc <= 1) {
        return(dval_error("Function 'and' must be passed at least 2 arguments."));
    }*/

    for (int i = 0; i < argc; i++) {
        if (args[i].type != DVAL_INT) { // TODO: I don't think I need this with the new arg type system
            return(dval_error("Function 'and' must be passed an integer for argument '%d'.", i));
        }
        if (args[i].integer != denv_get(a, "true")->integer) { // if False
            return denv_get(a, "false");
        }
    }

    return denv_get(a, "true");
}

// or - returns true (0) if given any given integer/double is true (0). Returns false (1) if no given integer/double is true (0).
dval *builtin_or(denv *a, dval *args, unsigned int argc) {
    /*if (argc <= 1) {
        return(dval_error("Function 'or' must be passed at least 2 arguments."));
    }*/

    for (int i = 0; i < argc; i++) {
        if (args[i].type != DVAL_INT) {
            return(dval_error("Function 'or' must be passed an integer for argument '%d'.", i));
        }
        if (args[i].integer == denv_get(a, "true")->integer) { // if True
            return denv_get(a, "true");
        }
    }

    return denv_get(a, "false");
}

bool print_elem(dval arg, bool removeQuotations) { // TODO: Use pointer to dval?
    switch (arg.type) {
        case DVAL_INT:
        {
            printf("%d", arg.integer);
            return true;
        } break;
        
        case DVAL_DOUBLE:
        {
            printf("%f", arg.doub);
            return true;
        } break;
        
        case DVAL_CHARACTER:
        {
            if (removeQuotations) {
                printf("%c", arg.character);
            } else {
                printf("'%c'", arg.character);
            }
            return true;
        } break;
        
        case DVAL_STRING:
        {
            if (removeQuotations) {
                printf("%s", arg.nstr);
            } else {
                printf("\"%s\"", arg.nstr);
            }
            return true;
        } break;
        
        case DVAL_FUNC:
        {
            printf("(Func)");
            return true;
        } break;
        
        case DVAL_LIST:
        {
            printf("[");
            for (int i = 0; i < arg.count; i++) {
                print_elem(arg.elements[i], false);
                if (i != arg.count - 1) {
                    printf(", ");
                }
            }
            printf("]");
            return true;
        } break;
        
        case DVAL_EXPR:
        {
            printf("(");
            for (int i = 0; i < arg.count; i++) {
                print_elem(arg.elements[i], false);
                if (i != arg.count - 1 && i != 0) {
                    printf(", ");
                } else if (i == 0) {
                    printf(" ");
                }
            }
            printf(")");
            return true;
        } break;
        
        case DVAL_QEXPR:
        {
            printf("{");
            for (int i = 0; i < arg.count; i++) {
                print_elem(arg.elements[i], false);
                if (i != arg.count - 1) {
                    printf(" ");
                }
            }
            printf("}");
            return true;
        } break;
        
        case DVAL_IDENT:
        {
            printf(".%s", arg.str);
            return true;
        } break;
        
        case DVAL_TYPEVALUE:
        {
            printf("%s", get_type_string(arg.typeValue));
            return true;
        } break;
        
        default:
        {
            return false;
        }
    }
}

dval *builtin_print(denv *e, dval *args, unsigned int argc) {
    for (int i = 0; i < argc; i++) {
        if (!print_elem(args[i], true)) {
            return(dval_error("Cannot print value of type Unknown or Any!"));
        }
        printf(" ");
    }
    printf("\n");
    return(denv_get(e, "true"));
}

dval *builtin_read(denv *e, dval *args, unsigned int argc) { // TODO: Bug with strings not being printed correctly using this expression: print (read "> "); // Note(Christian): Seems to have been fixed!
    /*if (argc < 1 || argc > 1) {
        return(dval_error("Function 'read' must be passed only 1 argument"));
    }
    if (args[0].type != DVAL_STRING) {
        return(dval_error("Function 'read' must be passed a string for argument 1"));
    }*/
    
    printf("%s", args[0].str);
    char input[255];
    fgets(input, 255, stdin);
    char *cpy = (char *) malloc(strlen(input) + 1);
    strcpy(cpy, input);
    cpy[strlen(cpy) - 1] = '\0';
    /*fgets(input, 255, stdin);
    char *pos;
    if ((pos=strchr(input, '\n')) != NULL)
     *pos = '\0';*/
    //scanf("%s", input);
    return(dval_string((char *) cpy));
}

dval *builtin_clear(denv *e, dval *args, unsigned int argc) {
#ifdef _WIN32
    system("cls"); // TODO: Change this?
#else
    linenoiseClearScreen();
#endif
    return(denv_get(e, "true"));
}

dval *builtin_exit(denv *e, dval *args, unsigned int argc) {
    running = false;
    return(denv_get(e, "true"));
}

internal void denv_add_builtin(denv *e, char *name, dbuiltin func, int argc, DVAL_TYPE *argTypes, DVAL_TYPE returnType, bool varargs) {
    dval *v = dval_func(func, argc, varargs, argTypes, returnType, 1);
    dval_del(denv_put(e, name, v, v->constant)); // NOTE: v is coppied (but not deleted in denv_put function)
    dval_del(v);
}

internal void denv_add_type(denv *e, char *name, DVAL_TYPE type) {
    dval *v = dval_type(type);
    dval_del(denv_put(e, name, v, v->constant));
    dval_del(v);
}

internal void denv_add_const(denv *e, char *name, dval *v) {
    v->constant = 1;
    dval_del(denv_put(e, name, v, 1));
    dval_del(v);
}

void denv_add_builtins(denv *e) { // TODO: Combine mallocs into one array so we're only doing 1 malloc rather than many
	DVAL_TYPE *add_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	add_argTypes[0] = DVAL_ANY;
    denv_add_builtin(e, "+", builtin_add, 1, add_argTypes, DVAL_ANY, true);

    DVAL_TYPE *subtract_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	subtract_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "-", builtin_subtract, 1, subtract_argTypes, DVAL_ANY, true);
 
    DVAL_TYPE *multiply_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	multiply_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "*", builtin_multiply, 2, multiply_argTypes, DVAL_ANY, true);
 
    DVAL_TYPE *divide_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	divide_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "/", builtin_divide, 2, divide_argTypes, DVAL_ANY, true);
 
    DVAL_TYPE *mod_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	mod_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "mod", builtin_mod, 2, mod_argTypes, DVAL_ANY, true);

    DVAL_TYPE *power_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	power_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "^", builtin_power, 2, power_argTypes, DVAL_ANY, true);

    DVAL_TYPE *power_argTypes2 = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	power_argTypes2[0] = DVAL_ANY;
	denv_add_builtin(e, "**", builtin_power, 2, power_argTypes2, DVAL_ANY, true);
 
    DVAL_TYPE *succ_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	succ_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "succ", builtin_succ, 1, succ_argTypes, DVAL_ANY, false);
 
 
    DVAL_TYPE *list_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	list_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "list", builtin_list, 0, list_argTypes, DVAL_LIST, true);

    DVAL_TYPE *len_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	len_argTypes[0] = DVAL_LIST;
	denv_add_builtin(e, "len", builtin_len, 1, len_argTypes, DVAL_INT, false);

    DVAL_TYPE *get_argTypes = (DVAL_TYPE *) malloc(2 * sizeof(DVAL_TYPE));
	get_argTypes[0] = DVAL_INT;
	get_argTypes[1] = DVAL_LIST;
	denv_add_builtin(e, "get", builtin_get, 2, get_argTypes, DVAL_ANY, false);

    DVAL_TYPE *set_argTypes = (DVAL_TYPE *) malloc(3 * sizeof(DVAL_TYPE));
	set_argTypes[0] = DVAL_INT;
	set_argTypes[1] = DVAL_ANY;
	set_argTypes[2] = DVAL_LIST;
	denv_add_builtin(e, "set", builtin_set, 3, set_argTypes, DVAL_ANY, false);

    DVAL_TYPE *first_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	first_argTypes[0] = DVAL_LIST;
	denv_add_builtin(e, "first", builtin_first, 1, first_argTypes, DVAL_ANY, false);

    DVAL_TYPE *last_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	last_argTypes[0] = DVAL_LIST;
	denv_add_builtin(e, "last", builtin_last, 1, last_argTypes, DVAL_ANY, false);

    DVAL_TYPE *head_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	head_argTypes[0] = DVAL_LIST;
	denv_add_builtin(e, "head", builtin_head, 1, head_argTypes, DVAL_LIST, false);

    DVAL_TYPE *tail_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	tail_argTypes[0] = DVAL_LIST;
	denv_add_builtin(e, "tail", builtin_tail, 1, tail_argTypes, DVAL_LIST, false);

    DVAL_TYPE *join_argTypes = (DVAL_TYPE *) malloc(2 * sizeof(DVAL_TYPE));
	join_argTypes[0] = DVAL_LIST;
	join_argTypes[1] = DVAL_LIST;
	denv_add_builtin(e, "join", builtin_join, 2, join_argTypes, DVAL_LIST, false); // TODO: Allow join to do more than 2 (varargs)
 

    DVAL_TYPE *def_argTypes = (DVAL_TYPE *) malloc(3 * sizeof(DVAL_TYPE));
	def_argTypes[0] = DVAL_IDENT;
	def_argTypes[1] = DVAL_TYPEVALUE;
	def_argTypes[2] = DVAL_ANY;
	denv_add_builtin(e, "def", builtin_def, 3, def_argTypes, DVAL_ANY, false);

    DVAL_TYPE *const_argTypes = (DVAL_TYPE *) malloc(3 * sizeof(DVAL_TYPE));
	const_argTypes[0] = DVAL_IDENT;
	const_argTypes[1] = DVAL_TYPEVALUE;
	const_argTypes[2] = DVAL_ANY;
	denv_add_builtin(e, "const", builtin_const, 3, const_argTypes, DVAL_ANY, false);

    DVAL_TYPE *typeof_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	typeof_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "typeof", builtin_typeof, 1, typeof_argTypes, DVAL_TYPEVALUE, false);

    DVAL_TYPE *cast_argTypes = (DVAL_TYPE *) malloc(2 * sizeof(DVAL_TYPE));
	cast_argTypes[0] = DVAL_TYPEVALUE;
	cast_argTypes[1] = DVAL_ANY;
	denv_add_builtin(e, "cast", builtin_cast, 2, cast_argTypes, DVAL_ANY, false);


    DVAL_TYPE *and_argTypes = (DVAL_TYPE *) malloc(2 * sizeof(DVAL_TYPE));
	and_argTypes[0] = DVAL_INT;
	and_argTypes[1] = DVAL_INT;
	denv_add_builtin(e, "and", builtin_and, 2, and_argTypes, DVAL_INT, true);

    DVAL_TYPE *or_argTypes = (DVAL_TYPE *) malloc(2 * sizeof(DVAL_TYPE));
	or_argTypes[0] = DVAL_INT;
	or_argTypes[1] = DVAL_INT;
	denv_add_builtin(e, "or", builtin_or, 2, or_argTypes, DVAL_INT, true);


    DVAL_TYPE *if_argTypes = (DVAL_TYPE *) malloc(3 * sizeof(DVAL_TYPE));
	if_argTypes[0] = DVAL_INT;
	if_argTypes[1] = DVAL_ANY;
	if_argTypes[2] = DVAL_ANY;
	denv_add_builtin(e, "if", builtin_if, 3, if_argTypes, DVAL_ANY, false);


    DVAL_TYPE *print_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	print_argTypes[0] = DVAL_ANY;
	denv_add_builtin(e, "print", builtin_print, 0, print_argTypes, DVAL_INT, true);

    DVAL_TYPE *read_argTypes = (DVAL_TYPE *) malloc(1 * sizeof(DVAL_TYPE));
	read_argTypes[0] = DVAL_STRING;
	denv_add_builtin(e, "read", builtin_read, 1, read_argTypes, DVAL_STRING, false);

	denv_add_builtin(e, "clear", builtin_clear, 0, (void *) 0, DVAL_INT, false);
	denv_add_builtin(e, "exit", builtin_exit, 0, (void *) 0, DVAL_INT, false);
    
    denv_add_type(e, "any", DVAL_ANY);
    denv_add_type(e, "int", DVAL_INT);
    denv_add_type(e, "double", DVAL_DOUBLE);
    denv_add_type(e, "char", DVAL_CHARACTER);
    denv_add_type(e, "string", DVAL_STRING);
    denv_add_type(e, "func", DVAL_FUNC);
    denv_add_type(e, "array", DVAL_LIST);
    denv_add_type(e, "type", DVAL_TYPEVALUE);
    
    denv_add_const(e, "true", dval_int(0));
    denv_add_const(e, "false", dval_int(1));
}
