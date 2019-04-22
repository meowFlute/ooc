# Chapter 2 - Dynamic Linkage — Generic Functions
### What is meant by "dynamic linkage"?
Paraphrasing what the author wrote, **dynamic linkage** or **late binding** determines the function to be called as late as possible — only during execution of the call, not before. That means that at compile time and when linking the executable, it still is not determined. As we will see, this facilitates the creation of generic functions that operate differently on different classes, which is called **polymorphism**, or **polymorphic functions**. Some examples of such functions we'll create in this chapter are:
- a constructor and destructor functions to be used by `new()` and `delete()` that initialize objects,
- a new version of `sizeof` that we'll call `sizeOf()` that won't just tell us the size of the pointer we gave it, but instead will accept a `const void *` to the `const struct Class *` in the object definition, and
- a function to "clone" the object into another identical but unique object with the same data
- a comparison function `differ(self, something)` that will change based on which class it is acting on.

In doing so we'll create the notion of a **class** that is aware of its own public and private members can be instantiated like a data type

### How do we use dynamic linkage in object-oriented C?

The basic format we're going to use here is very similar to how GLib organizes classes and objects. Each class will be made up of two structs, one to describe the size and provide base functionality to the class and one to instantiate the class. This example of a string shows it well:

```C
struct Class {
    size_t size;
    void * (* ctor) (void * self, va_list * app);
    void * (* dtor) (void * self);
    void * (* clone) (const void * self);
    int (* differ) (const void * self, const void * b);
};
struct String {
    const void * class; /* must be first */
    char * text;
};
```

A single unique `const struct Class *` would be created to point to the appropriate functions for that class, such that a string would use a different constructor function than a set.

You wind up using pointers like a mad man, so buckle up. For instance, an example of `new()` that utilizes the unique class constructor to instantiate an object looks like this:

```C
void * new (const void * _class, ...)
{
    const struct Class * class = _class;
    void * p = calloc(1, class —> size);
    assert(p);
    * (const struct Class **) p = class;
    if (class —> ctor)
    {
        va_list ap;
        va_start(ap, _class);
        p = class —> ctor(p, & ap);
        va_end(ap);
    }
    return p;
}
```
When you allocate memory with calloc in this function and get a pointer back to the memory, that pointer points to something that needs to be instantiated as a to a pointer to a class. Organizing it this way allows you to assume that the memory location at `*object` contains a pointer to the class description of that object, which tells you anything you need to know about making or destroying that object and how big it is.

In the image below, p is a pointer to the object. It takes a while to get used to, but it makes a lot of sense after thinking about it for a while.

![Pointer to pointer to class](http://i.imgur.com/tcvbHYw.png)

Ultimately that is the whole chapter in terms of general information — the rest is implementation specific between strings and something called atoms.

### Implementation example - `String`

The author moved all code that is common across classes to new.h/r/c. He uses the convention new.r as a private header file that contains the definition of the `Class` struct. This means individual classes like `String` are dependent on this private header in String.c, but that users of `String` class are not dependent on it. It contains a struct definition that can be implemented by each class, as we'll see when we take a look at String.c

```C
#ifndef	CLASS_R
#define	CLASS_R

#include <stdarg.h>
#include <stdio.h>

struct Class {
	size_t size;
	void * (* ctor) (void * self, va_list * app);
	void * (* dtor) (void * self);
	void * (* clone) (const void * self);
	int (* differ) (const void * self, const void * b);
};

#endif
```

String.h contains only a `const void * String` to the String definition struct that can be passed into `void * new (const void * class, ...)`

```C
#ifndef	STRING_H
#define	STRING_H

extern const void * String;

#endif
```
This is because, as we saw above, this struct points to a class description that contains pointers to implementations of all class methods. This `const void * String` carries a lot of info.

String.c contains an implementation of the `struct Class` that represents the `extern const void * String` referred to in String.h. It also contains a `struct String` that represents the implementable object. The functions String_ctor/dtor/etc are defined in the space where I've located ellipses, we'll cover those next.

```C
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "String.h"
#include "new.h"
#include "new.r"

struct String {
	const void * class;	/* must be first */
	char * text;
};

(...)

static const struct Class _String = {
	sizeof(struct String),
	String_ctor,
	String_dtor,
	String_clone,
	String_differ
};

const void * String = & _String;
```

Looking at the above setup may lead you to scroll back up to the pointer diagram. We have a `struct String` type of implementable struct, and a `const void * String` which points to a `static const struct Class` implementation named `_String`. Looking at the constructor for this class **String_ctor()** along side **new()** sheds some light on the difference and how they interact.

```C
void * new (const void * _class, ...)
{
    const struct Class * class = _class;
	void * p = calloc(1, class -> size);

	assert(p);
	* (const struct Class **) p = class;

	if (class -> ctor)
	{	va_list ap;

		va_start(ap, _class);
		p = class -> ctor(p, & ap);
		va_end(ap);
	}
	return p;
}

static void * String_ctor (void * _self, va_list * app)
{
    struct String * self = _self;
	const char * text = va_arg(* app, const char *);

	self -> text = malloc(strlen(text) + 1);
	assert(self -> text);
	strcpy(self -> text, text);
	return self;
}
```

When we call `s = new(String);`, we're really saying `s = new(&_String)`, so `const struct Class * class = _class;` in **new()** is receiving our class definition struct directly. To implement a `struct String` object, we made `class->size` represent `sizeof(struct String)`, so `void * p = calloc(1, class -> size);` is truly implementing space for and returning a pointer to a zero'd out `struct String`, not `struct Class _String`. Since we already have an appropriate `struct Class` for `struct String`'s first member, and the first member of the struct is at the memory location of that struct, we can set the `struct String` p is pointing at to a pointer to `_String`. We use a variable length argument list from `<stdarg.h>` as an input to the constructor **String_ctor()**.

**String_ctor()** casts the p pointer to a `struct String` and calls that `self`, because that is exactly what it was all along. As soon as **calloc()** returned a pointer to something that was the size of a `struct String` we had implemented an object. **String_ctor()** needs only to read in arguments from `va_list ap` and use them to initialize `self`.

The rest of the functions use the fact that we know a pointer to a `struct String` is also a pointer to `const struct Class * _String` (since that is the first member of `struct String`). **delete()** uses **String_dtor()** to **free()** the second member `char * text` before **free()**ing the `struct String`

```C
void delete (void * self)
{
    const struct Class ** cp = self;

	if (self && * cp && (* cp) -> dtor)
		self = (* cp) -> dtor(self);
	free(self);
}

static void * String_dtor (void * _self)
{
    struct String * self = _self;

	free(self -> text), self -> text = 0;
	return self;
}
```
Looking at the rest of the functions in the files in this folder completes the loop on what is going on here. It is very smart!

### Exercises
>To see polymorphic functions in action we need to implement Object and Set with
dynamic linkage. This is difficult for Set because we can no longer record in the set
elements to which set they belong.
>
>There should be more methods for strings: we need to know the string length,
we want to assign a new text value, we should be able to print a string. Things get
interesting if we also deal with substrings.
>
>Atoms are much more efficient, if we track them with a hash table. Can the
value of an atom be changed?
>
>String_clone() poses an subtle question: in this function String should be the
same value as self −> class. Does it make any difference what we pass to new()?
