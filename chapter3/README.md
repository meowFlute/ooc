# Chapter 3 - Programming Savvy — Arithmetic Operations
This whole chapter appears to be an ode to what you can do with dynamic linkage.
We shall see if I'm correct...

### What We're Doing
We are making a text interpreter that can do arithmetic operations like add and
subtract from input from the command line. The goal here is to show how dynamic linkage
accomplished two main things:
1. it allows for compartmentalization of the problem
2. it allows for code reuse and generalization

### parse.c
In this example parse.c contains the main function and the rules for parsing that we'll later extend through dynamic linkage. To understand what it is we're doing here with the three different ways we'll extend parse.c we need to understand what is going on in parse.c first and foremost.

parse.c contains a use of error handling from <setjmp.h> that is a form of error handling that those who use languages with try-catch loops may not be used to. It allows for immediate recovery from bad inputs by setting a recovery point using `setjmp()` and then going to that recovery point whenever an error is caught using `longjmp()`. To do this, the author creates a generic function to be called that prints a message and then calls `longjump()`, and this is found below in `error()`. One use of error catching is found in the main loop, where is the input contains too many numbers, like "4 + 4 4" there will be an extra token and the message "trash after sum" will be printed before jumping back to `setjmp()`. This then increments the `volatile int errors` before going back into the input loop.

If the input loop is broken by making `fgets()` return NULL (in linux with control+D to generate an EOF character). The presence of errors is then returned as shown below.

```C
static jmp_buf onError;

int main (void)
{
    volatile int errors = 0;
	char buf [BUFSIZ];

	if (setjmp(onError))
		++ errors;

	while (fgets(buf, sizeof buf, stdin))
		if (scan(buf))
		{
            void * e = sum();

			if (token)
				error("trash after sum");
			process(e);
			delete(e);
		}

	return errors > 0;
}

void error (const char * fmt, ...)
{
    va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap), putc('\n', stderr);
	va_end(ap);
	longjmp(onError, 1);
}
```
![an example of the main loop](http://i.imgur.com/zwBAdCL.png)

Delving further into what this main function is doing, we need to look next at `scan()`. `scan()` uses a special header <errno.h> that is able to catch errors from the ANSI-C function `strtod()`. If for instance, 1e+999999999999999999999999999999999 is passed into the main function, we use the `error()` to report that we can't fit that number into a double.

One funny aspect that the author skimmed over is that in this case with 'n' being the value for NUMBER, we can trick the program by feeding it a token of 'n'. Since number is global, whatever is in there will stay there and n will just be the last value of number.

Other than that, we grab whatever it is that isn't whitespace, put it in the global variable token, with the possibility of it going into number with a token of 'n'. This makes `scan()` nice and generic, so we can write code to process as many or as few tokens as we'd like to be able to handle.

**Note: if `scan(0)` is called, bp will not be updated but merely advanced since it is a static pointer and in that case buf will be 0**

```C
#include <errno.h>
#include <stdlib.h>

enum tokens {				/* must not clash with operators */
	NUMBER = 'n'			/* literal constant */
};

static enum tokens token;	/* current input symbol */
static double number;		/* if NUMBER: numerical value */cds

static enum tokens scan (const char * buf)  /* return token = next input symbol */
{
    static const char * bp;

	if (buf)
		bp = buf;			/*  new input line */

	while (isspace(* bp & 0xff))
		++ bp;
	if (isdigit(* bp & 0xff) || * bp == '.')
	{
        errno = 0;
		token = NUMBER, number = strtod(bp, (char **) & bp);
		if (errno == ERANGE)
			error("bad value: %s", strerror(errno));
	}
	else
		token = * bp ? * bp ++ : 0;
	return token;
}
```
![scan examples](http://i.imgur.com/4U8bmYq.png)

`sum()` is called next in the main function after we've grabbed the first token. The author writes the following summing up all possible operator tokens:
>At the top level expressions are recognized by the function sum() which internally calls on scan() and returns a representation that can be manipulated by process()
and reclaimed by delete().
>
>If we do not use yacc we recognize expressions by the method of recursive descent where grammatical rules are translated into equivalent C functions. For example: a sum is a product, followed by zero or more groups, each consisting of
an addition operator and another product.
>
>There is a C function for each grammatical rule so that rules can call each other.
Alternatives are translated into switch or if statements, iterations in the grammar
produce loops in C. The only problem is that we must avoid infinite recursion.
>
>token always contains the next input symbol. If we recognize it, we must call
scan(0) to advance in the input and store a new symbol in token.

All functions that are extended to parse operators are shown below along with Value.h. Let's consider a few of these case by case so we can see what is going on here and how this becomes something that is pretty extensible through dynamic linkage.

First we'll think about a correctly formatted addition operation. The first stored token is already a correctly formatted number from what happened prior in the main function. `sum()` calls `product()` which calls `factor()`. In `factor()` we call `result = new(Value, number);`, and call `scan(0);` again. Here `scan(0)` finds a '+' and loads that into token before returning. We return result from the call of `factor()`, so now we're back in `product()` with `result = new(Value, number);` and since `token` is '+' we return result right back into `sum()`. In sum we set type to point at the const void * `Add` (which we'll later reveal to be a sort of object), call `scan(0)` which loads in the next number, and then return `result = new(type, result, product())`. `product()` will go find the next number the same way it did with the first one and also find the end of the buffer loading 0 in as the next token. In this case we could expand this final returned value from `sum()` to `result = new(Add, new(Value, first_number), new(Value, second_number));`.

So it will be with all two term cases. For instance division will be, `result = new(Div, new(Value, first_number), new(Value, second_number));`. The unary `Minus` will just be `new(Minus, number)` and a value by itself will be `new(Value, number)`. But what about larger operations? What if we did something like `(3+(5/7))*4`?

`result = new(Mult,new(Add,new(Value,3),new(Div,new(Value,5),new(Value,7))),new(Value,4));`

Pretty slick when you think about it!

The main function then calls `process()` and `delete()` which are ready for extension in Value.h. This wraps up parse.c.

```C
/* Value.h */
#ifndef	VALUE_H
#define	VALUE_H

/*
 *	node types
 */

const void * Minus;
const void * Value;
const void * Mult;
const void * Div;
const void * Add;
const void * Sub;

/*
 *	tree management
 */

void * new (const void * type, ...);
void process (const void * tree);
void delete (void * tree);

#endif

/* parse.c */
static void * sum (void);

static void * factor (void)
{
    void * result;

	switch ((int)token) {
	case '+':
		scan(0);
		return factor();
	case '-':
		scan(0);
		return new(Minus, factor());
	default:
		error("bad factor: '%c' 0x%x", token, token);
	case NUMBER:
		result = new(Value, number);
		break;
	case '(':
		scan(0);
		result = sum();
		if (token != ')')
			error("expecting )");
	}
	scan(0);
	return result;
}

static void * product (void)
{
    void * result = factor();
	const void * type;

	for (;;)
	{
        switch ((int)token) {
		case '*':
			type = Mult;
			break;
		case '/':
			type = Div;
			break;
		default:
			return result;
		}
		scan(0);
		result = new(type, result, factor());
	}
}

static void * sum (void)
{
    void * result = product();
	const void * type;

	for (;;)
	{
        switch ((int)token) {
		case '+':
			type = Add;
			break;
		case '-':
			type = Sub;
			break;
		default:
			return result;
		}
		scan(0);
		result = new(type, result, product());
	}
}
```

### value.c

So now we're ready for some dynamic linkage. Why dynamic linkage in this case? The author describes the issue below:

>How do we process an expression? If we only want to perform simple arithmetic on numerical values, we can extend the recognition functions and compute the result as soon as we recognize the operators and the operands: sum() would
expect a double result from each call to product(), perform addition or subtraction as soon as possible, and return the result, again as a double function value.
>
>If we want to build a system that can handle more complicated expressions we need to store expressions for later processing. In this case, we can not only perform arithmetic, but we can permit decisions and conditionally evaluate only part of
an expression, and we can use stored expressions as user functions within other expressions. All we need is a reasonably general way to represent an expression.
The conventional technique is to use a binary tree and store token in each node:
>
>```C
struct Node {
    enum tokens token;
    struct Node * left, * right;
};
>```
>
> This is not very flexible, however. We need to introduce a union to create a node in which we can store a numerical value and we waste space in nodes representing unary operators. Additionally, process() and delete() will contain switch statements which grow with every new token which we invent.

So the stage is set, we want to use our previously learned dynamic linkage principles to take the function calls we introduced earlier that generate our binary tree and roll with them. Recall the following example of `(3+(5/7))*4`

`result = new(Mult,new(Add,new(Value,3),new(Div,new(Value,5),new(Value,7))),new(Value,4));`

Imagine result points to a '*' operator with two child nodes, '+' and 4. '+' branches down to 3 and '/', which branches down to 5 and 7

![tree image](http://i.imgur.com/0bo0zUM.png)

We need a way to store this stuff and then process it. Since these are all binary operators we'll focus on those first.

```C
struct Type {
	void * (* new) (va_list ap);
	double (* exec) (const void * tree);
	void (* delete) (void * tree);
};

void * new (const void * type, ...)
{
    va_list ap;
	void * result;

	assert(type && ((struct Type *) type) -> new);

	va_start(ap, type);
	result = ((struct Type *) type) -> new(ap);
	* (const struct Type **) result = type;
	va_end(ap);
	return result;
}
```

You see above that new takes in a type that must contain three key pieces of information.
1. How to do `type->new()`
2. How to do `type->exec()`
3. How to do `type->delete()`

For our Add only, that involves the following

```C
struct Bin {
	const void * type;
	void * left, * right;
};

static void * mkBin (va_list ap)
{
	struct Bin * node = malloc(sizeof(struct Bin));

	assert(node);
	node -> left = va_arg(ap, void *);
	node -> right = va_arg(ap, void *);
	return node;
}

static double doAdd (const void * tree)
{
	return exec(((struct Bin *) tree) -> left) +
			exec(((struct Bin *) tree) -> right);
}

static void freeBin (void * tree)
{
	delete(((struct Bin *) tree) -> left);
	delete(((struct Bin *) tree) -> right);
	free(tree);
}

static struct Type _Add = { mkBin, doAdd, freeBin };

const void * Add = & _Add;
```

You can probably see how this works for other binary operators, since they need only change the `type->exec()` and add an additional type...

```C
static double doSub (const void * tree)
{
	return exec(((struct Bin *) tree) -> left) -
		exec(((struct Bin *) tree) -> right);
}

static double doMult (const void * tree)
{
	return exec(((struct Bin *) tree) -> left) *
		exec(((struct Bin *) tree) -> right);
}

static double doDiv (const void * tree)
{
    double left = exec(((struct Bin *) tree) -> left);
	double right = exec(((struct Bin *) tree) -> right);

	if (right == 0.0)
		error("division by zero");
	return left / right;
}

static struct Type _Sub = { mkBin, doSub, freeBin };
static struct Type _Mult = { mkBin, doMult, freeBin };
static struct Type _Div = { mkBin, doDiv, freeBin };

const void * Sub = & _Sub;
const void * Mult = & _Mult;
const void * Div = & _Div;
```

To walk this tree we need to know how numbers are made and return too.

```C
struct Val
{
	const void * type;
	double value;
};

static void * mkVal (va_list ap)
{
    struct Val * node = malloc(sizeof(struct Val));

	assert(node);
	node -> value = va_arg(ap, double);
	return node;
}

static double doVal (const void * tree)
{
	return ((struct Val *) tree) -> value;
}
```

So in essence, what is evaluated is just what we type in. The beauty of it is that we need only create new types when we want to extend more tokens, we don't need to modify `new()`, `process()` or `delete()` or even `exec()`. Another nice thing is that each individual function and storage unit is very simple.

To complete the description, take a look at the rest of the file value.c

```C
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "value.h"

/*
 *	evaluation driver
 */

struct Type {
	void * (* new) (va_list ap);
	double (* exec) (const void * tree);
	void (* delete) (void * tree);
};

void * new (const void * type, ...)
{	va_list ap;
	void * result;

	assert(type && ((struct Type *) type) -> new);

	va_start(ap, type);
	result = ((struct Type *) type) -> new(ap);
	* (const struct Type **) result = type;
	va_end(ap);
	return result;
}

static double exec (const void * tree)
{
	assert(tree && * (struct Type **) tree
		&& (* (struct Type **) tree) -> exec);

	return (* (struct Type **) tree) -> exec(tree);
}

void process (const void * tree)
{
	printf("\t%g\n", exec(tree));
}

void delete (void * tree)
{
	assert(tree && * (struct Type **) tree
		&& (* (struct Type **) tree) -> delete);

	(* (struct Type **) tree) -> delete(tree);
}

/*
 *	NUMBER
 */

struct Val {
	const void * type;
	double value;
};

static void * mkVal (va_list ap)
{	struct Val * node = malloc(sizeof(struct Val));

	assert(node);
	node -> value = va_arg(ap, double);
	return node;
}

static double doVal (const void * tree)
{
	return ((struct Val *) tree) -> value;
}

/*
 *	unary operators
 */

struct Un {
	const void * type;
	void * arg;
};

static void * mkUn (va_list ap)
{	struct Un * node = malloc(sizeof(struct Un));

	assert(node);
	node -> arg = va_arg(ap, void *);
	return node;
}

static double doMinus (const void * tree)
{
	return - exec(((struct Un *) tree) -> arg);
}

static void freeUn (void * tree)
{
	delete(((struct Un *) tree) -> arg);
	free(tree);
}

/*
 *	binary operators
 */

struct Bin {
	const void * type;
	void * left, * right;
};

static void * mkBin (va_list ap)
{
	struct Bin * node = malloc(sizeof(struct Bin));

	assert(node);
	node -> left = va_arg(ap, void *);
	node -> right = va_arg(ap, void *);
	return node;
}

static double doAdd (const void * tree)
{
	return exec(((struct Bin *) tree) -> left) +
			exec(((struct Bin *) tree) -> right);
}

static double doSub (const void * tree)
{
	return exec(((struct Bin *) tree) -> left) -
		exec(((struct Bin *) tree) -> right);
}

static double doMult (const void * tree)
{
	return exec(((struct Bin *) tree) -> left) *
		exec(((struct Bin *) tree) -> right);
}

static double doDiv (const void * tree)
{	double left = exec(((struct Bin *) tree) -> left);
	double right = exec(((struct Bin *) tree) -> right);

	if (right == 0.0)
		error("division by zero");
	return left / right;
}

static void freeBin (void * tree)
{
	delete(((struct Bin *) tree) -> left);
	delete(((struct Bin *) tree) -> right);
	free(tree);
}

/*
 *	Types
 */

static struct Type _Add = { mkBin, doAdd, freeBin };
static struct Type _Sub = { mkBin, doSub, freeBin };
static struct Type _Mult = { mkBin, doMult, freeBin };
static struct Type _Div = { mkBin, doDiv, freeBin };
static struct Type _Minus = { mkUn, doMinus, freeUn };
static struct Type _Value = { mkVal, doVal, free };

const void * Add = & _Add;
const void * Sub = & _Sub;
const void * Mult = & _Mult;
const void * Div = & _Div;
const void * Minus = & _Minus;
const void * Value = & _Value;
```
