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
