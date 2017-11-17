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

#include <structure.h>
#include <stb_sprintf.h>
//#include <gb.h>

dval *dval_int(long integer) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_INT;
    d->constant = 0;
    d->integer = integer;
    return(d);
}

dval *dval_double(double doub) {
    dval *v = (dval *) malloc(sizeof(dval));
    v->type = DVAL_DOUBLE;
    v->constant = 0;
    v->doub = doub;
    return(v);
}

dval *dval_character(char character) {
    dval *v = (dval *) malloc(sizeof(dval));
    v->type = DVAL_CHARACTER;
    v->constant = 0;
    v->character = character;
    return(v);
}

dval *dval_string(char *str) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_STRING;
    d->constant = 0;
    
    d->str = (char *) malloc(strlen(str) + 1);
    strcpy(d->str, str);
    //d->nstr = gb_string_make(gb_heap_allocator(), str);
    
    return d;
}

dval *dval_error(char *fmt, ...) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_ERROR;
    
    va_list va;
    va_start(va, fmt);
    d->str = (char *) malloc(512);
    //vsnprintf(d->str, 511, fmt, va);
    
    stbsp_vsnprintf(d->str, 511, fmt, va);
    
    d->str = (char *) realloc(d->str, strlen(d->str) + 1);// TODO: What is this for?
    va_end(va);
    
    return d;
}

dval *dval_info(char *fmt, ...) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_INFO;
    
    va_list va;
    va_start(va, fmt);
    d->str = (char *) malloc(512);
    //vsnprintf(d->str, 511, fmt, va);
    
    stbsp_vsnprintf(d->str, 511, fmt, va);
    
    d->str = (char *) realloc(d->str, strlen(d->str) + 1); // TODO: What is this for?
    va_end(va);
    
    return d;
}

dval *dval_func(dbuiltin func, int argc, bool varargs, DVAL_TYPE *argTypes, int constant) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_FUNC;
    d->constant = constant;
	d->func = (dval_func_info) { func, argTypes, argc, varargs };
    return(d);
}

dval *dval_list(dval *elements, unsigned int count) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_LIST;
    d->constant = 0;
    d->count = count;
    d->elements = elements; // NOTE: This get's calloc'd in the read_eval_expr function.
    return(d);
}

dval *dval_type(DVAL_TYPE type) {
    dval *d = (dval *) malloc(sizeof(dval));
    d->type = DVAL_TYPEVALUE;
    d->constant = 1;
    d->typeValue = type;
    
    return(d);
}

dval *dval_copy(dval *d) { // TODO: Add list and string
    dval *v = (dval *) malloc(sizeof(dval));
    v->type = d->type;
    v->constant = d->constant;
    
    switch (d->type) {
        case DVAL_INT:
        {
            v->integer = d->integer;
        } break;

        case DVAL_DOUBLE:
        {
            v->doub = d->doub;
        } break;

        case DVAL_ERROR:
        {
            v->str = d->str; // TODO: Is this correct? Shouldn't I have to copy the string?
        } break;

		case DVAL_FUNC: // TODO: Shouls I be copying the dval types?
        {
			DVAL_TYPE *argTypes = (DVAL_TYPE *) malloc(d->func.argc * sizeof(DVAL_TYPE));
			int for_length = d->func.argc;
			if (for_length == 0 && d->func.varargs) for_length = 1; // There has to be at least 1 type is varargs is turned on.
			for (int i = 0; i < for_length; i++) {
				argTypes[i] = d->func.argTypes[i];
			}
			v->func = (dval_func_info) { d->func.func, argTypes, d->func.argc, d->func.varargs };
        } break;

        case DVAL_ANY:
        {
            v->type = DVAL_ERROR;
            v->str = "(Interpreter Error) Cannot copy dval of type ANY.";
        } break;

        case DVAL_TYPEVALUE:
        {
            v->typeValue = d->typeValue;
        } break;

        case DVAL_LIST:
        {
            v->count = d->count;
            v->elements = calloc(v->count, sizeof(dval));
            for (int i = 0; i < d->count; i++) {
                v->elements[i] = d->elements[i];
            }
        } break;

        case DVAL_STRING:
        {
            v->str = (char *) malloc(strlen(d->str) + 1);
            strcpy(v->str, d->str);
        } break;

        default:
        {
            v->type = DVAL_ERROR;
            v->str = "(Interpreter Error) Cannot copy dval of Unknown type.";
        }
    }
    
    return(v);
}

void dval_del(dval *d) {
    switch(d->type) {
        case DVAL_ERROR:
        case DVAL_IDENT:
        case DVAL_INFO:
        {
            free(d->str);
        } break;
        case DVAL_STRING:
        {
            free(d->str);
            //gb_string_free(d->nstr);
        } break;
        break;
		case DVAL_FUNC:
		{
			free(d->func.argTypes);
		} break;
    }
    free(d); // Error Handling?
}

denv *denv_new(void) {
    denv *e = (denv*) malloc(sizeof(denv));
    e->parent = NULL;
    e->map = Hashmap_create(NULL, NULL);
    if (e->map == NULL) {
        fprintf(stderr, "Error: Failed to create hashmap for variable definitions!");
        exit(EXIT_FAILURE);
    }
    return(e);
}

void denv_del(denv *e) {
    Hashmap_destroy(e->map);
    free(e);
}

int running = true;
