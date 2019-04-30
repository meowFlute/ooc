# Chapter 4

### What are we doing?

We're going to start playing around with inheritance for the first time. Turns out to be a pretty simple concept where we just start our struct with the class we want to inherit from. In this sense, everything we've done so far starts from the same base class.

As an example we use a Circle class that inherits from a Point class.

![Circle from Point example](http://i.imgur.com/mUTurmt.png)

```C
/* Base class defining dynamically linkable functions */
struct Class {
	size_t size;
	void * (* ctor) (void * self, va_list * app);
	void * (* dtor) (void * self);
	void (* draw) (const void * self);
};

/* Point class capable of extending those functions */
struct Point {
	const void * class;
	int x, y;				/* coordinates */
};
static const struct Class _Point = { sizeof(struct Point), Point_ctor, 0, Point_draw };
const void * Point = & _Point;

/* Circle class that contains Point + unique (overridden) dynamic linkage and extra data */
struct Circle {
    const struct Point _;
    int rad;                /* radius */
};
static const struct Class _Circle = { sizeof(struct Circle), Circle_ctor, 0, Circle_draw };
const void * Circle = & _Circle;
```

### What we aren't doing yet - Metaclasses

That comes in chapter 6. That is when we figure out how have different lists of dynamically linkable functions instead of whatever is defined in new.c/h/_private.h

### So what do we learn in this chapter?

For now, let's just focus on the way inheritance is used in the constructor of `Circle`

```C
static void * Circle_ctor (void * _self, va_list * app)
{
	struct Circle * self =
		((const struct Class *) Point) -> ctor(_self, app);

	self -> rad = va_arg(* app, int);
	return self;
}
```

we allow the `Point` constructor to read off of the variable list of parameters first and then follow it up with the `Cirlce` specific construction. This means we reused most of `Point`'s constructor code! Same can be used with tons of stuff, anything you can imagine. Destructors work backwards.

### Casting between classes

`move()` found in Point.c/h is a great example of the ability to caste between classes. You can pass a `void *` pointing to a `Circle` to this function and it will work perfectly.  

In this case `move()` is a statically linked (instead of dynamically linked) class method. This means that it **CANNOT** be overridden.

```C
void move (void * _self, int dx, int dy)
{
	struct Point * self = _self;

	self -> x += dx, self -> y += dy;
}
```

### Access functions

To avoid needing to reach into the subclass' struct, we define something like this macro for access to public paramters.

```C
#define	get_x(p)	(((const struct Point *)(p)) -> x)
#define	get_y(p)	(((const struct Point *)(p)) -> y)
```

Or, the author points out, we could ommit the `const` and write a setting macro

```C
#define set_x(p,v)  (((struct Point *)(p)) —> x = (v))
#define set_y(p,v)  (((struct Point *)(p)) —> y = (v))
```

### Is It or Has It?

In this case our Circle is also a Point. It doesn't contain a Point - it *IS* a Point. You can cast it as a Point and use it as a Point. A Rectangle object however, might *HAVE* pointers to four Points.

from the text:

>Inheritance, i.e., making a subclass from a superclass, and aggregates, i.e.,
including an object as component of some other object, provide very similar func-
tionality. Which approach to use in a particular design can often be decided by the
is-it-or-has-it? test: if an object of a new class is just like an object of some other
class, we should use inheritance to implement the new class; if an object of a new
class has an object of some other class as part of its state, we should build an
aggregate.
>
>As far as our points are concerned, a circle is just a big point, which is why we
used inheritance to make circles. A rectangle is an ambiguous example: we can
describe it through a reference point and the side lengths, or we can use the end-
points of a diagonal or even three corners. Only with a reference point is a rectan-
gle some sort of fancy point; the other representations lead to aggregates. In our
arithmetic expressions we could have used inheritance to get from a unary to a
binary operator node, but that would substantially violate the test.

### Using the Included Code

This time you need to pass in arguements of p to points or c/p to circles. All the code does is show you a circle *IS* a point that can move and it overrides `draw()`.

![example usage](http://i.imgur.com/8R7LLRy.png)
