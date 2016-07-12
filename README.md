# Lydrige
## Introduction ##
Lydrige is an interpreted programming language.Therefore, it is generally slower than compiled languages. It is a fairly simple language inspired by Lisp and other functional languages. This makes is suitable for scripting and simple math related computations. While this language is inspired by Lisp, there are many things that make it different from lisp, for example there are no macros. Instead, macros are replaced by a simple version of q-expressions.

## Contents ##
* Basic Syntax
* Builtin Functions
* Data Types
* Examples
* Compiling the Interpreter and Running Examples
* Sublime Syntax Definitions for Lydrige

## Basic Syntax ##
The syntax is similar to the syntax in Lisp. You have an expression that has other expressions or data inside and separated by spaces. When evaluating an expression, the first item should be a function. This is, however, not needed for List Literals and Q-Expressions.
Here is an example of the syntax of a simple print statement:
`(print "This number evaluates to" (+ 1 1))`. This print statement returns `()` but does print out the arguments to the screen before the return.
* The syntax has now been slightly modified. Basically, files are made up of statements OR expression. A statement is defined as multiple expressions with a semicolon at the end. However, statements are evaluated as if they were one big expression. This allows us to have a syntax more similar to that of C-based langauges. Here is the print statement using the new syntax. It is also important to note that the above syntax and this new syntax will both be allowed by the interpreter and the result will be the same.
`print "This number evaluates to" (+ 1 1);`

## Builtin Functions ##
Here are the builtin functions in the language. Many of these builtin functions are very common, therefore they were written directly into the interpreter rather than a library. Note that this list does not include the basic operators and conditionals (ex: +, -, \*, /, %, ^ (power), ==, >, <, >=, <=, !=), however, they do exist within the langauge.
* `list` - returns a q-expression with all of the given arguments inside.
* `first` - returns the first item in a given q-expression or list literal.
* `last` - returns the last item in a given q-expression or list literal.
* `head` - returns all but the last item from a given q-expression or list literal in a new q-expression.
* `tail` - returns all but the first item from a given q-expression or list literal in a new q-expression.
* `join` - joins two lists together into a single list.
* `len` - returns the length of a given list.
* `print` - prints text to the screen. Returns `1`

## Data Types ##
Here are all of the data types in Lydrige and how you represent them within the language:
* `long` - simple number (ex: `12`)
* `double` - number with decimal (ex: `12.0`)
* `char` - character surrounded by single quotes (ex: `'c'`) *(Comming Soon)*
* `list` - a list whose children are evaluated, but not the list itself (ex: `[+ 1 (+ 1 1)]` returns `[+ 1 2]`)

## Examples ##
There are examples of programs/functions written in this language in the `examples directory`. This directory also includes *prelude.lydg*, the Prelude for the language; Is is auto-loaded for the REPL and every program. You can also find files for each of the Standard Library "modules" in the `stdlib directory`.

## Compiling the Interpreter and Running Examples ##
Currently, you can only compile on Linux (with gcc) or Mac OS X (with mac, gcc is aliased to clang). You simple run `make debug` to make the debug build or `make release` to make the release build. To run the debug build, enter `make run-debug`, and for the release build, enter `make run`.

## Sublime Syntax Definitons for Lydrige ##
I have also created a .tmLanguage file that will allow you to use Lydrige inside of Sublime Text. You simply put it in your `~/.config/sublime-text-3/Packages/User` folder.