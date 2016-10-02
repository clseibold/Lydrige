# Lydrige
<!-- vscode-markdown-toc -->
![travis-ci](https://travis-ci.org/krixano/Lydrige.svg?branch=dev)

## Contents
1. [Introduction](#Introduction-0)
2. [Basic Syntax](#BasicSyntax-1)
3. [Builtin Functions](#BuiltinFunctions-2)
4. [Data Types](#DataTypes-3)
5. [Examples](#Examples-4)
6. [Compiling the Interpreter and Running Examples](#CompilingtheInterpreterandRunningExamples-5)
7. [Sublime Syntax Definitons for Lydrige](#SublimeSyntaxDefinitonsforLydrige-6)

<!-- /vscode-markdown-toc -->
##  1. <a name='Introduction-0'></a>Introduction
Lydrige is a statically typed interpreted programming language inspired by lisp. It is a fairly simple language which makes it suitable for scripting and simple math related computations. While this language is inspired by Lisp, there are many things that make it different from lisp, for example there are no macros. Instead, macros are replaced by a simple version of q-expressions.

You can find even more information about this language on my website at [http://krixano.x10host.com/Lydrige](http://krixano.x10host.com/Lydrige), which includes:
* Documentation on the Prelude, the Standard Library, and Builtin Functions
* Documentation on how many of the builtin data structures are implemented into the language
* Pre-Compiled downloads for the 3 major Operating Systems and x86/x86_64 architectures
* Instructions on how to compile yourself
* And Changelogs for all versions of the language, including alpha versions

##  2. <a name='BasicSyntax-1'></a>Basic Syntax
The syntax is similar to the syntax in Lisp. You have an expression that has other expressions or data inside and separated by spaces. When evaluating an expression, the first item should be a function. This is, however, not needed for List Literals and Q-Expressions.
Here is an example of the syntax of a simple print statement:
`(print "This number evaluates to" (+ 1 1))`. This print statement returns `()` but does print out the arguments to the screen before the return.
* The syntax has now been slightly modified. Basically, files are made up of statements OR expression. A statement is defined as multiple expressions with a semicolon at the end. However, statements are evaluated as if they were one big expression. This allows us to have a syntax more similar to that of C-based langauges. Here is the print statement using the new syntax. It is also important to note that the above syntax and this new syntax will both be allowed by the interpreter and the result will be the same.
`print "This number evaluates to" (+ 1 1);`

You can view an example of a simple program in this language in the [examples/test.lydg](http://github.com/christianap/Lydrige/blob/dev/examples/test.lydg) file. This file explains many of the different features of the language.

##  3. <a name='BuiltinFunctions-2'></a>Builtin Functions
Here are the builtin functions in the language. Many of these builtin functions are very common, therefore they were written directly into the interpreter rather than a library. Note that this list does not include the basic operators and conditionals (ex: +, -, \*, /, %, ^ (power), ==, >, <, >=, <=, !=), however, they do exist within the langauge.
* `list`  - returns a q-expression with all of the given arguments inside. (`list 5 4 4 3;` returns `[5, 4, 4, 3]`)
* `first` - returns the first item in a given q-expression or list literal. (`first [5, 4, 4, 3];` returns `5`)
* `last`  - returns the last item in a given q-expression or list literal. (`last [5, 4, 4, 3];` returns `3`)
* `head`  - returns all but the last item from a given q-expression or list literal in a new q-expression. (`head [5, 4, 4, 3];` returns `[5, 4, 4]`)
* `tail`  - returns all but the first item from a given q-expression or list literal in a new q-expression. (`tail [5, 4, 4, 3];` returns `[4, 4, 3]`)
* `join`  - joins two lists together into a single list. (`join [5, 4] [4, 3];` returns `[5, 4, 4, 3]`)
* `get`   - gets item from given list at given index. (`get 0 [5, 4, 4, 3];` returns `5`)
* `set`   - returns a new q-expression with the item at the given index of given q-expression set to a given value. Doesn't modify original q-expression. (`set 0 4 [5, 4, 4, 3];` returns `[4, 4, 4, 3]`)
* `len`   - returns the length of a given list. (`len [5, 4, 4, 3];` returns `4`)
* `print` - prints text to the screen. Returns `1` (`print [5, 4, 4, 3] 23.5;` will print `[5, 4, 4, 3] 23.5` and return `1`)

##  4. <a name='DataTypes-3'></a>Data Types
Here are all of the data types in Lydrige and how you represent them within the language:
* `long`   - simple number (ex: `12`)
* `double` - number with decimal (ex: `12.0`)
* `char`   - character surrounded by single quotes (ex: `'c'`)
* `list`   - a list whose children are evaluated, but not the list itself. They are implemented as a value that allocates its elements on the heap in contiguous memory. They are not dynamic! (ex: `[+, 1, (+ 1 1)]` returns `[+, 1, 2]`)

##  5. <a name='Examples-4'></a>Examples
There are examples of programs/functions written in this language in the `examples directory`. This directory also includes *prelude.lydg*, the Prelude for the language; Is is auto-loaded for the REPL and every program. You can also find files for each of the Standard Library "modules" in the `stdlib directory`. 

Until Lydrige is fully rewritten, these examples won't currently work. However, some examples that do currently work are provided below:
* `print 5.2;`
* `print (+ 3 4.4);`
* `print (join [5, 4, 3, 3] [2, 2, 3]);`
* `print (+ 1 (get 0 [3, 2, 2]));`

##  6. <a name='CompilingtheInterpreterandRunningExamples-5'></a>Compiling the Interpreter and Running Examples
Currently, you can only compile on Linux (with gcc) or Mac OS X (with mac, gcc is aliased to clang). You simple run `make debug` to make the debug build or `make release` to make the release build. To run the debug build, enter `make run-debug`, and for the release build, enter `make run`.

##  7. <a name='SublimeSyntaxDefinitonsforLydrige-6'></a>Sublime Syntax Definitons for Lydrige
I have also created a .tmLanguage file that will allow you to use Lydrige inside of Sublime Text. You simply put it in your `~/.config/sublime-text-3/Packages/User` folder.
