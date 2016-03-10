# Lydrige
## Introduction ##
Lydrige is an interpreted programming language built off of and expanded from the interpreter from the BuildYourOwnLisp book (by Mr Daniel Holden). Since this programming language is interpreted, it is generally slower than other compiled languages. Lydrige is a fairly simple higher level language inspired by Lisp. This makes is suitable for scripting and simple math related computations. While this language is inspired by Lisp, there are many things that make it different from lisp, for example there are no macros. Instead, macros are replaced by a simple version of q-expressions. You can see future ideas and plans in the [ideas.md file](https://github.com/christianap/Lydrige/blob/master/ideas.md "ideas.md file").

## Contents ##
* Basic Syntax
* Builtin Functions
* Data Types
* Examples
* Compiling the Interpreter and Running Examples

## Basic Syntax ##
The syntax is similar to the syntax in Lisp. You have an expression that has other expressions or data inside and separated by spaces. When evaluating an expression, the first item should be a function. This is, however, not needed for List Literals and Q-Expressions.
Here is an example of the syntax of a simple print statement:
`(print "This number evaluates to " (+ 1 1))`. This print statement returns `()` but does print out the arguments to the screen before the return.

## Builtin Functions ##
Here are the builtin functions in the language. Many of these builtin functions are very common, therefore they were written directly into the interpreter rather than a library. Note that this list does not include the basic operators and conditionals (+, -, \*, /, %, ^ (power), ==, >, <, >=, <=, !=)
* `list` - returns a q-expression with all of the given arguments inside.
* `first` - returns the first item in a given q-expression or list literal.
* `last` - returns the last item in a given q-expression or list literal.
* `head` - returns all but the last item from a given q-expression or list literal in a new q-expression.
* `tail` - returns all but the first item from a given q-expression or list literal in a new q-expression.
* `eval` - evaluates a q-expression (by converting it into an s-expression).
* `join` - joins two q-expressions or lists together into a single q-expression.
* `len` - returns the length of a given q-expression or list.
* `typeof` - returns a string of the type of the given item.
* `def` - defines a variable in global scope/root environment. The variable is mutable (simply use def again with same variable name).
* `const` - similar to def, but defines variable as immutable. If variable already existed (and not immutable), the value is change and is made immutable.
* `let` - similar to def, but defines variable in local scope/current environment
* `let_const` - defines variable in local scope/current environment. The variable is immutable. (will be implemented soon!)
* `l` - creates a lambda with the first given q-expression as the argument names and the second q-expression as the body
* `lambda` - alias to l
* `if` - if given integer is 1, the first given q-expression is evaluated, otherwise the second given q-expression is evaluated
* `and` - returns 1 if all given integers/doubles are 1. Returns 0 if any given integer/double is 0.
* `or` - returns 1 if given any given integer/double is 1. Returns 0 if no given integer/double is 1.
* `print` - prints text to the screen. Returns ()
* `error` - returns an error
* `load` - loads in a separate file
* `exit` - exits the REPL (currently not working)
* `inner_eval` - acts as if the given q-expression is a list literal. The items are evaluated, but the expression itself is not.
* `get` - gets item from given q-expression or list literal at given index. (ex: `get 0 {2 3 4 5}` will return `2` , `get 0 {unbound}` will return an error)
* `extract` - very similar to the `get` function, except does not automatically evaluate the expression (this allows you to get unbound symbols without error, for example: `list (extract 0 {unbound})` will return `{unbound}`. This is needed for the future static typing of the language.)
* `set` - returns a new q-expression with the item at the given index of give q-expression or list literal to a given value.

## Data Types ##
Here are all of the data types in Lydrige and how you represent them within the language:
* `long` - simple number (ex: `12`)
* `double` - number with decimal (ex: `12.0`)
* `byte` - hex (ex: `0x10`)
* `char` - character surrounded by single quotes (ex: `'c'`)
* `string` - quotes wrapped around one or more characters (ex: `"string"`)
* `range` - A range of numbers. (ex: `5..20` is a range of numbers 5 through 20, but not including 20) (Not currently working)
* `list` - a list whose children are evaluated, but not the list itself (ex: `[+ 1 (+ 1 1)]` returns `[+ 1 2]`)
* `special list` - just like a regular list, but are evaluated when inside q-expressions (ex: `{'[+ (+ 1 1) 3]}` returns `{[+ 2 3]}`)
* `q-expression` - a list whose children are NOT evaluated, neither the list itself (ex: `{+ 1 (+ 1 1)}` returns `{+ 1 (+ 1 1)}`)
* `special q-expression` - a list where neither the children, nor the list itself is evaluated. This special version does not allow the evaluation of any special s-expressions or special lists unless this q-expression is returned or used in a function. An example use of this is when you want to set a function that returns a q-expression, but the special lists/s-expressions inside are only evaluated when the function is called rather than defined. You can see an example of this in the standard library (`./examples/stdlib.lydg`):
```clojure
	(const_fn {get_fn_arg_checking bare_args arg_types} {
		; '{} are special q-expressions that will be converted to regular q-expressions when returned/passed to a function,
		;   This allows you to have special s-expressions or special lists that won't evaluate until the special q-expression is turned into a regular q-expression!
		'{if (== (typeof '(extract 0 bare_args)) '(get 0 arg_types))}
	})
```
* `s-expression` - a list that is automatically evaluated, first item must be a function (ex: `(+ 1 (+ 1 1))` returns `3`)
* `special s-expression` - just like a regular s-expression, but are evaluated when inside q-expressions (ex: `{3 '(+ 1 1)}` returns `{3 2}`)
* `lambda` - called a function within the interpreter code (ex: `(\ {x y} {print x y})`)

## Examples ##
There are examples of programs/functions written in this language in the examples directory. This directory also includes *newstdlib.dnc*, which will be the Prelude for the language (use the one prefixed with *new-*, the other one is an older version).

## Compiling the Interpreter and Running Examples ##
Compile the interpreter by running `./gradlew mainReleaseExecutable` in the terminal (Unix) or `gradlew.bat mainReleaseExecutable` in the cmd (Windows). In order for this project to compile, make sure you have a C/C++ compiler installed(gcc, clang, mingw, visual studio, xcode, or cygwin). To run the program, execute `./run` (Unix) or `./run.bat` (Windows) in the project root. If you want to run one of the examples, simply pass the file as an argument to the *run* or *run.bat* file, for example: `./run ./examples/stdlib.lydg`.
