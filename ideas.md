 This file lists ideas of language features and changes.


## Static Typing syntax: (built into interpreter, based on strings and typeof) ##
This is inferred: `def {name} 5`

Explicit: `def {name int} 5`

':' is a new syntax to the language that tells the
interpreter what type the symbol should be when being defined. MAYBE?
This is also used to report errors when trying to change the symbol to another value. You can specify that the symbol should be dynamic by giving it a nil type:

`def {name nil} "Any Value can Go Here"`

Lambda's can be also be statically typed for their return value(s) and argument(s):

`def {name func} (\ {var int} {+ var 1} int)`


 An easy way to handle errors:
       `if_err (inner_eval {testing 4}) {err} {return}`
 * err is a variable that has been assigned to the error that was found.
 * return is a variable that has been assigned to the return value of the first argument (expr).
 The second argument (q-expr/list) is evaluated when an error is found.
 The third argument (q-expr/list) is evaluated when an error was not found.


 ## While Loops ##
 Should they be automatically wrapped around "do" function, so you can have mutiple s-expressions?
`
(while {var int} {== var (len "hello")} {
    ; They return something, where does the returned thing go?
   	; The variable var is declared as a local variable within this while scope
   	; Notice that you can use var inside the conditional. Because of this, the conditional
   	;   is in a q-expression so it doesn't evaluate before the new scope, or before the var
   	;   is declared.
   	; Also note that this while function does do type checking for its given variables.
})

(while {== 1 (len "hello")} [
  	; This is done untill 1 equals the length of "hello"
   	; Notice that this while loop can take a list literal as the body because
   	;   there are no local variables being used within the scope. This will
   	;   evaluate all of the children in the body before the while loop starts.
   	; Note: using a local variable requires that the body be in a q-expression,
   	;   since having it as a list will try to evaluate any variables being used
   	;   and will return an error because the variable wasn't declared at the time of evaluation.
])

(while [== 1 (len "hello")] [
   	; Since this while loop has no variables, you can also give the conditional
   	;   as a list. This will make sure that the length of the list is evaluated
   	;   before the while loop starts running. This is usefull
   	;   if you don't want to continually evaluate the children of the conditional
   	;   expression, especially because of performance reasons.
])

(while (== 1 (len "hello")) [
   	; This method evaluates all of the conditional expression before the while loop
   	;   starts. This is usefull if nothing within the conditional
   	;   will change from code within the while loop.
   	; Do not use this if the conditional changes every iteration of the loop!
])

(while {i int} [< var (len "hello")] {
   	; Notice that we can get the functionality of a for loop by simply adding 1 (or more)
   	;   to the variable at the end of an iteration.
   	; Also note that [] were used for the conditional so (len "hello") is evaluated
   	;   before the while loop starts. If we had the conditional in {}, the expression
   	;   (len "hello") would evaluate with each iteration instead.
   	; Since you are changing the variable local to while, the body should NOT be in
   	;   a list literal (also because changing i depends on what it was before).
   	(= i (+ i 1))
 })
`