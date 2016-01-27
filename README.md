# Lydrige
## Introduction ##
Lydrige is an interpreted programming language built off of and expanded from the interpreter from the BuildYourOwnLisp book (by Mr Daniel Holden). Since this programming language is interpreted, it is generally slower than other compiled languages. Lydrige is a fairly simple higher level language inspired by Lisp. This makes is suitable for scripting and simple math related computations. While this language is inspired by Lisp, there are many things that make it different from lisp, for example there are no macros. Instead, macros are replaced by a simple version of q-expressions. You can see future ideas and plans in the [ideas.md file](https://github.com/christianap/Lydrige/blob/master/ideas.md "ideas.md file").

## Basic Syntax ##
The syntax is similar to the syntax in Lisp. You have an expression that has other expressions or data inside and separated by spaces. When evaluating an expression, the first item should be a function. This is, however, not needed for List Literals and Q-Expressions.
Here is an example of the syntax of a simple print statement:
`(print "This number evaluates to " (+ 1 1))`. This print statement returns `()` but does print out the arguments to the screen before the return.

## Builtin Functions ##
Here are the builtin functions in the language. Many of these builtin functions are very common, therefore they were written directly into the interpreter rather than a library. Note that this list does not include the basic operators and conditionals (+, -, *, /, %, ^ (power), ==, >, <, >=, <=, !=).
* `list` - returns a q-expression with all of the given arguments inside.
* `first` - returns the first item in a given q-expression or list literal.
* `last` - returns the last item in a given q-expression or list literal.
* `head` - returns all but the last item from a given q-expression or list literal in a new q-expression.
* `tail` - returns all but the first item from a given q-expression or list literal in a new q-expression.
* `eval` - evaluates a q-expression (by converting it into an s-expression).
* `join` - joins two q-expressions together.
* `len` - returns the length of a given q-expression or list.
* `typeof` - returns a string of the type of the given item.
* `def` - defines a variable in global scope/root environment. The variable is mutable (simply use def again with same variable name).
* `const` - similar to def, but defines variable as immutable. If variable already existed (and not immutable), the value is change and is made immutable.
* `=` - similar to def, but defines variable in local scope/current environment
* `\` - creates a lambda with the first given q-expression as the argument names and the second q-expression as the body
* `lambda` - alial to \
* `if` - if given integer is 1, the first given q-expression is evaluated, otherwise the second given q-expression is evaluated
* `and` - returns 1 if all given integers/doubles are 1. Returns 0 if any given integer/double is 0.
* `or` - returns 1 if given any given integer/double is 1. Returns 0 if no given integer/double is 1.
* `print` - prints text to the screen. Returns ()
* `error` - returns an error
* `load` - loads in a separate file
* `exit` - exits the REPL (currently not working)
* `inner_eval` - acts as if the given q-expression is a list literal. The items are evaluated, but the expression itself is not.
* `get` - gets item from given q-expression or list literal at given index, does not automatically evaluate the expression (this allows you to get unbound symbols without error, for example: `list (get 0 {unbound})` will return `{unbound}`. This is needed for the future static typing of the language.)
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
* `q-expression` - a list whose children are NOT evaluated, neither the list itself (ex: `{+ 1 (+ 1 1)}` returns `{+ 1 (+ 1 1)}`)
* `s-expression` - a list that is automatically evaluated, first item must be a function (ex: `(+ 1 (+ 1 1))` returns `3`)
* `lambda` - called a function within the interpreter code (ex: `(\ {x y} {print x y})`)

## Examples ##
There are examples of programs/functions written in this language in the examples directory. This directory also includes *stdlib.dnc*, which will be the Prelude.

## Compiling the Interpreter and Running Examples ##
You can compile the interpreter by simply running make. The file outputs to *build/main*. All of the examples are copied directly into the build folder so you can easily run the files from the interpreter. You do this by typing `./main ./file.dnc`. If you just want to start the REPL, simply run the file without any arguments.
