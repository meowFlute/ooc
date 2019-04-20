# Chapter 1 - Abstract Data Types
### Why? What do we even mean by 'Abstract'?

**The simple answer we'll see below is that it allows us to chop up the application into little bite size chunks.**

Explaining what is meant by abstraction in this context, the author writes:
> We call a data type abstract, if we do not reveal its representation to the user.

So if you create a class in C++ called a `Dog`, you might give it a name and a few other features, and then provide a way to interact with the dog through a list of member functions that include construction and destruction. `Dog ralph = new Dog("ralph");` might be a way to create a dog, and `ralph.feed();` might be a way to feed this new dog named ralph. None of the details of implementation of those functions in Dog.cpp need to be revealed to the user. What happens in the dog library can be a secret and the dog class can still be useful.

The author continues:

>At a
theoretical level this requires us to specify the properties of the data type by
mathematical axioms involving the possible operations. For example, we can
remove an element from a queue only as often as we have added one previously,
and we retrieve the elements in the same order in which they were added.
>
> Abstract data types offer great flexibility to the programmer. Since the
representation is not part of the definition, we are free to choose whatever is easiest or most efficient to implement. If we manage to distribute the necessary information correctly, use of the data type and our choice of implementation are totally
independent.
>
>Abstract data types satisfy the good programming principles of information hiding and divide and conquer. Information such as the representation of data items is
given only to the one with a need to know: to the implementer and not to the user.
With an abstract data type we cleanly separate the programming tasks of implementation and usage: we are well on our way to decompose a large system into
smaller modules.

So we proceed to in a simple way implement a class of our own as an example for this first chapter.

### Example Abstract Objects - `Set` and `Object`
Going forward, we are going to create an abstract object that can do a few things. It will be a `Set` of generic objects. The functionality of a `Set` will include
1. It will have a constructor/destructor
1. It will be able to add other objects
1. It will be able to drop other objects
1. It will have a find function that finds a specific element in the `Set`
1. It will have a boolean function that returns true if a specific element is present in the `Set`

A few tools I wasn't necessarily aware of will be utilized here and I'll do my best to explain them as we go.

First the basic way to create a Set.h header file in C is the way we'll go forward here, with the distinction that we'll also include a reference to an external representation of the `Set` object to be implemented in the Set.c file. You'll note that all of the functions that could return any object are `void *` functions and that all of the arguments are also `void *`. This allows `Set` to be a set of literally anything. In chapter 8 we cover dynamic type checking, so at that point it would be enforced to only have one type. For now a set could have any collection of any objects.

```C
#ifndef SET_H
#define SET_H

extern const void * Set;

void * add (void * set, const void * element);
void * find (const void * set, const void * element);
void * drop (void * set, const void * element);
int contains (const void * set, const void * element);

#endif
```
Since every conceivable object is going to need a constructor/destructor, we declare those in a different header that all abstract objects will include in their .c files.

```C
#ifndef	NEW_H
#define	NEW_H

void * new (const void * type, ...);
void delete (void * item);

#endif
```
Here new and delete will use some form of memory management to allocate space for whatever we throw at it. That is why each object has an `extern const void *`, these are used to define the size of memory that an abstract object will consume. In a sense, these objects `Object` and `Set` inherit these functions.

And we need something to put in our `Set`, so we define a generic `Object` with a boolean function that compares two `Object` objects

```C
#ifndef	OBJECT_H
#define	OBJECT_H

extern const void * Object;

int differ (const void * a, const void * b);

#endif
```
Since we're focused on abstract objects, let's implement a main.c file knowing only what is shown above. **It shouldn't matter to us how these functions or objects are actually implemented**. That is the point that is being made with the abstract parts of this.
```C
#include <stdio.h>

#include "new.h"
#include "Object.h"
#include "Set.h"

int main ()
{
    void * s = new(Set);
	void * a = add(s, new(Object));
	void * b = add(s, new(Object));
	void * c = new(Object);

	if (contains(s, a) && contains(s, b))
		puts("ok");

	if (contains(s, c))
		puts("contains?");

	if (differ(a, add(s, a)))
		puts("differ?");

	if (contains(s, drop(s, a)))
		puts("drop?");

	delete(drop(s, b));
	delete(drop(s, c));

	return 0;
}
```
So again, I realize we haven't covered the details of how these functions are implemented, but that is the point. Object oriented programming is a paradigm, and we've adopted the abstraction part of it here. All we know is that we can make new `Object` and `Set` abstract objects, and that we can compare `Object` objects and put them into `Set` objects. How that happens isn't part of the main.c file at all. In fact, main.c will compile into main.o with no issues at all, **independent of the implementation of `Set` and `Object`**

In fact, for actual implementation we can use any number of implementations of both of these abstract objects. One possibility is Set.c, which is not shown in this summary but is included in the author's source code. For his own pedagogical reasons, he implemented functions that only contain the base functionality and nothing more. His second example is Bag.c, and this example provides a much better framework for understanding how abstract objects might be implemented from generic c objects in my opinion.

If you look below, you'll see that we define structs called `Set` and `Object` and also constant void pointers called the same exact names.

**new()** here works like an empty constructor. It just uses calloc() to allocate a block of memory that is the size of whatever object that gets passed in and set it to zero — it doesn't initialize any member information (though it could).

**delete()**: Remember, free just needs the void pointer returned from calloc(), the delete function becomes pretty simple.

**add()**: Here the author decided to let the element keep track of what set it is in. The set it just a struct with a count after all.

**find()**: Since the element keeps track of what `Set` it is in, this function just verifies that the set pointer passed into the function matches the set pointer in element.

the rest of the functions explain themselves.
****
```C
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "new.h"
#include "Set.h"
#include "Object.h"

struct Set { unsigned count; };
struct Object { unsigned count; struct Set * in; };

static const size_t _Set = sizeof(struct Set);
static const size_t _Object = sizeof(struct Object);

const void * Set = & _Set;
const void * Object = & _Object;

void * new (const void * type, ...)
{
    const size_t size = * (const size_t *) type;
	void * p = calloc(1, size);

	assert(p);
	return p;
}

void delete (void * item)
{
	free(item);
}

void * add (void * _set, const void * _element)
{
    struct Set * set = _set;
	struct Object * element = (void *) _element;

	assert(set);
	assert(element);

	if (! element -> in)
		element -> in = set;
	else
		assert(element -> in == set);
	++ element -> count, ++ set -> count;

	return element;
}

void * find (const void * _set, const void * _element)
{
    const struct Object * element = _element;

	assert(_set);
	assert(element);

	return element -> in == _set ? (void *) element : 0;
}

int contains (const void * _set, const void * _element)
{
	return find(_set, _element) != 0;
}

void * drop (void * _set, const void * _element)
{
    struct Set * set = _set;
	struct Object * element = find(set, _element);

	if (element)
	{	if (-- element -> count == 0)
			element -> in = 0;
		-- set -> count;
	}
	return element;
}

unsigned count (const void * _set)
{
    const struct Set * set = _set;

	assert(set);
	return set -> count;
}

int differ (const void * a, const void * b)
{
	return a != b;
}
```
### Conclusions
One question that may still linger is why use so many void * when they type seems to be known? In this case the author is showing a way to set up a class that can be modified **INFINITELY** without needing to change how someone like main.c is using it. Take a look at Set.c below to see what I mean. This compiles just fine in the source code! The output from main only differs because of the added functionality of count used in Bag.c. Removing that from Bag.c provides an exact corollary to Set.h
```C
#include <assert.h>
#include <stdio.h>

#include "new.h"
#include "Set.h"
#include "Object.h"

const void * Set;
const void * Object;

#if ! defined MANY || MANY < 1
#define	MANY	10
#endif

static int heap [MANY];

void * new (const void * type, ...)
{
    int * p;							/* & heap[1..] */

	for (p = heap + 1; p < heap + MANY; ++ p)
		if (! * p)
			break;
	assert(p < heap + MANY);
	* p = MANY;
	return p;
}

void delete (void * _item)
{
    int * item = _item;

	if (item)
	{	assert(item > heap && item < heap + MANY);
		* item = 0;
	}
}

void * add (void * _set, const void * _element)
{
    int * set = _set;
	const int * element = _element;

	assert(set > heap && set < heap + MANY);
	assert(* set == MANY);
	assert(element > heap && element < heap + MANY);

	if (* element == MANY)
		* (int *) element = set - heap;
	else
		assert(* element == set - heap);

	return (void *) element;
}

void * find (const void * _set, const void * _element)
{
    const int * set = _set;
	const int * element = _element;

	assert(set > heap && set < heap + MANY);
	assert(* set == MANY);
	assert(element > heap && element < heap + MANY);
	assert(* element);

	return * element == set - heap ? (void *) element : 0;
}

int contains (const void * _set, const void * _element)
{
	return find(_set, _element) != 0;
}

void * drop (void * _set, const void * _element)
{
    int * element = find(_set, _element);

	if (element)
		* element = MANY;
	return element;
}

int differ (const void * a, const void * b)
{
	return a != b;
}
```
### Exercises
This is the only code I'm going to implement myself in this chapter is the single exercise at the end. In this exercise, the author suggests a linked list might be a useful representation of a `Set`, so that an `Object` can be part of several sets simultaneously. Another benefit is that the `Set` can be implemented without knowing anything about the `Object`. I intend to divorce the `Set` and `Object` into their own .c files, and implement a debugging tool the author suggested. The full exercise description is given below:

>If an object can belong to several sets simultaneously, we need a different
representation for sets. If we continue to represent objects as small unique integer
values, and if we put a ceiling on the number of objects available, we can represent
a set as a bitmap stored in a long character string, where a bit selected by the
object value is set or cleared depending on the presence of the object in the set.
>
>A more general and more conventional solution represents a set as a linear list
of nodes storing the addresses of objects in the set. This imposes no restriction on
objects and permits a set to be implemented without knowing the representation of
an object.
>For debugging it is very helpful to be able to look at individual objects. A reasonably general solution are two functions
>
>`int store (const void * object, FILE * fp);`
>
>and
>
>`int storev (const void * object, va_list ap);`
>
>**store()** writes a description of the object to the file pointer. **storev()** uses **va_arg()**
to retrieve the file pointer from the argument list pointed to by ap. Both functions
return the number of characters written. **storev()** is practical if we implement the
following function for sets:
>
>`int apply (const void * set,
int (* action) (void * object, va_list ap), ...);`
>
>**apply()** calls **action()** for each element in set and passes the rest of the argument
list. **action()** must not change set but it may return zero to terminate **apply()** early.
**apply()** returns true if all elements were processed.
