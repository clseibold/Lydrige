# Lydrige (Version 0.6.0 alpha)
<!-- vscode-markdown-toc -->
[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](https://github.com/krixano/Lydrige/blob/master/LICENSE)
[![Build Status](https://travis-ci.org/krixano/Lydrige.svg?branch=master)](https://travis-ci.org/krixano/Lydrige)
[![Stories in Ready](https://badge.waffle.io/krixano/Lydrige.svg?label=ready&title=Ready)](http://waffle.io/krixano/Lydrige) 

To Download, Goto the Releases page [here](https://github.com/krixano/Lydrige/releases) <br>
Currently, only the linux version of v0.5.0 can be downloaded from here.

## Contents
1. [Introduction](#Introduction-0)
2. [Basic Syntax](#BasicSyntax-1)
3. [Builtin Functions](#BuiltinFunctions-2)
4. [Data Types](#DataTypes-3)
5. [Examples](#Examples-4)
6. [Compiling the Interpreter and Running Examples](#CompilingtheInterpreterandRunningExamples-5)
   * [Linux/Mac](#Compiling-Linux_Mac)
   * [Windows](#Compiling-Windows)
   * [FreeBSD](#Compiling-FreeBSD)
7. [Sublime Text tmLanguage File](#sublime-text-tmlanguage-7)
7. [License](#License-8)

<!-- /vscode-markdown-toc -->
##  1. <a name='Introduction-0'></a>Introduction
Lydrige is a simple statically typed interpreted programming language inspired by Lisp. Although originally inspired by Lisp, there are some changes which make the Syntax of the language appear more C-like. Right now the language supports: functions, lists, list-manipulation, input, string, casting, casting a string into different values (string to int, etc.), and more!

You can find even more information about this language on my website at [http://krixano.x10host.com/Lydrige](http://krixano.x10host.com/Lydrige), which includes:
* Documentation on the Prelude, the Standard Library, and Builtin Functions
* Documentation on how many of the builtin data structures are implemented into the language
* Pre-Compiled downloads for the 3 major Operating Systems and x86/x86_64 architectures
* Instructions on how to compile yourself
* And Changelogs for all versions of the language, including alpha versions

##  2. <a name='BasicSyntax-1'></a>Basic Syntax
The syntax is similar to the syntax in Lisp. You have an expression that has other expressions or data inside and is separated by spaces. When evaluating an expression, the first item should be a function. This is, however, not needed for List Literals and Q-Expressions.
Here is an example of the syntax of a simple print statement:
`(print "This number evaluates to" (+ 1 1))`. This print statement returns `()` but does print out the arguments to the screen before the return.
* The syntax has now been slightly modified. Basically, files are made up of statements. A statement is defined as an identifier (or lambda) followed by multiple expressions with a semicolon at the end. However, a statement is evaluated as if it were one big expression. This allows us to have a syntax more similar to that of C-based langauges. Here is the print statement using the new syntax.
`print "This number evaluates to" (+ 1 1);`

You can view an example of what a simple program would look like in version 0.6.0 when this version is complete in the [examples/0.6.0NewSyntax.lydg](https://github.com/krixano/Lydrige/blob/master/examples/0.6.0NewSyntax.lydg) file. This file explains many of the different features of the language and includes changes in version 0.6.0a. This version is not ready to be able to interpret this file yet. However, there are some examples that will work with the current feature-set in the `Examples` section below.

##  3. <a name='BuiltinFunctions-2'></a>Builtin Functions
Here are the builtin functions in the language. Many of these builtin functions are very common, therefore they were written directly into the interpreter rather than a library. Note that this list does not include the basic operators and conditionals (ex: +, -, \*, /, %, ^ (power), ==, >, <, >=, <=, !=), however, they do exist within the langauge.
* `list`   - returns a q-expression with all of the given arguments inside. (`list 5 4 4 3;` returns `[5, 4, 4, 3]`)
* `first`  - returns the first item in a given q-expression or list literal. (`first [5, 4, 4, 3];` returns `5`)
* `last`   - returns the last item in a given q-expression or list literal. (`last [5, 4, 4, 3];` returns `3`)
* `head`   - returns all but the last item from a given q-expression or list literal in a new q-expression. (`head [5, 4, 4, 3];` returns `[5, 4, 4]`)
* `tail`   - returns all but the first item from a given q-expression or list literal in a new q-expression. (`tail [5, 4, 4, 3];` returns `[4, 4, 3]`)
* `join`   - joins two lists together into a single list. (`join [5, 4] [4, 3];` returns `[5, 4, 4, 3]`)
* `get`    - gets item from given list at given index. (`get 0 [5, 4, 4, 3];` returns `5`)
* `set`    - returns a new q-expression with the item at the given index of given q-expression set to a given value. Doesn't modify original q-expression. (`set 0 4 [5, 4, 4, 3];` returns `[4, 4, 4, 3]`)
* `len`    - returns the length of a given list. (`len [5, 4, 4, 3];` returns `4`)
* `typeof` - returns the type of the given value. (`typeof [5, 4, 4, 3];` returns `array`)
* `print`  - prints text to the screen. Returns `1` (`print [5, 4, 4, 3] 23.5;` will print `[5, 4, 4, 3] 23.5` and return `1`)

##  4. <a name='DataTypes-3'></a>Data Types
Here are all of the data types in Lydrige and how you represent them within the language:
* `int`   - simple number (ex: `12`)
* `double` - number with decimal (ex: `12.0`)
* `char`   - character surrounded by single quotes (ex: `'c'`)
* `string` - strings are surrounded by double quotes (ex: `"this is a string"`)
* `list`   - a list whose children are evaluated, but not the list itself. They are implemented as a value that allocates its elements on the heap in contiguous memory. They are not dynamic! (ex: `[+, 1, (+ 1 1)]` returns `[+, 1, 2]`)
* `qexpression` - a list in which its children and itself are not evaluated until the qexpression is evaluated. These are very similar to Quotes in Scheme. (ex: `{+ 3 (+ 3 3)}` doesn't evaluate, unless function it's passed to evaluates it)
* `qidentifier` (aka `symbol` or `dotted identifier`) - An identifier that acts like a qexpression. The identifier is *not* automatically evaluated until passed to the eval function, or a function that does this (for example, the `def` function). Note that the syntax has been recently changed to use a dot instead of a quote. Therefore you can call these `dotted indentifiers`. (ex: `.ident_name` is an identifier that is not evaluated until passed into a function that does so) (Former Syntax: `'ident_name`)

##  5. <a name='Examples-4'></a>Examples
There are examples of programs/functions written in this language in the `examples directory`. Note that this directory contains code that only works for v0.5.0! It will not work with this version! This directory also includes *prelude.lydg*, the Prelude for the language, which is auto-loaded in the REPL and in every lydrige program (although, this is not the case for v0.6.0 *until* the interpreter has been fully rewritten). You can also find files for each of the Standard Library "modules" in the `stdlib directory`. A sample of how I envision the language to look when v0.6.0 is released can be found in the `examples/0.6.0NewSyntax.lydg` file.

Until Lydrige is fully rewritten, these examples won't currently work. However, some examples that do currently work are provided below:
* `print 5.2;`
* `print (+ 3 4.4);`
* `print (join [5, 4, 3, 3] [2, 2, 3]);`
* `print (+ 1 (get 0 [3, 2, 2]));`
* `print {3 3 unbound (unbound_also 3 3)};`
* `print .symbol .also_known_as_dotted_identifier;`

##  6. <a name='CompilingtheInterpreterandRunningExamples-5'></a>Compiling the Interpreter and Running Examples
### <a name='Compiling-Linux_Mac'></a>Linux/Mac (requires gcc or clang)
Run `make debug` to make the debug build or `make release` to make the release build. To run the debug build, enter `make run-debug`, and for the release build, enter `make run`.

### <a name='Compiling-Windows'></a>Windows (requires mingw/mingw-w64 or clang)
**GCC:** Run `build.bat gcc x64` for 64-bit, or `build.bat gcc x86` for 32-bit.<br>
**Clang:** Run `build.bat clang x64` for 64-bit, or `build.bat clang x86` for 32-bit.<br>
**MSVC:** Run `build.bat msvc x64` for 64-bit, or `build.bat` for 32-bit.

To start the program, run `build.bat run`.

### <a name='Compiling-FreeBSD'></a>FreeBSD (requires devel/gmake)
You must install `devel/gmake` in order to use the Makefile. Run `gmake debug` to make the debug build or `gmake release` to make the release build. To run the debug build, enter `gmake run-debug`, and for the release build, enter `gmake run`.

## 7. <a name='sublime-text-tmlanguage-7></a>Sublime Text tmLanguage File
I have provided a tmLanguage file to get Syntax Highlighting for Lydrige in Sublime Text. It is not complete yet, as there are still a few bugs and other things to implement.

##  8. <a name='License-8'></a>License
See LICENSE.md and LICENSE-3RD-PARTY.txt

Lydrige Version v0.6.0a
Copyright (c) 2016-2017, Christian Seibold All Rights Reserved.
Under MIT License

Uses MPC Library under the BSD-2-Clause License
Copyright (c) 2013, Daniel Holden All Rights Reserved
https://github.com/orangeduck/mpc/

Uses Linenoise Library under the BSD-2-Clause License
Copyright (c) 2010-2014, Salvatore Sanfilippo <antirez at gmail dot com>
Copyright (c) 2010-1013, Pieter Noordhuis <pcnoordhuis at gmail dot com>
https://github.com/antirez/linenoise/
