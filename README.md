# Object Oriented C
## Some notes and thoughts about [this](https://www.cs.rit.edu/~ats/books/ooc.pdf) book

I am an embedded C programmer by trade but I used to program desktop applications. Recently an idea has been swirling around in my head that involves object oriented programming in an embedded environment. The compilers for the target MCUs I'm thinking about using come in one of two options:
1. Assembly
2. C

I was surprised to find online that a lot of programmers haven't ever been subjected to such hardware constraints, so they poo-poo the idea of writing object oriented code in C. After taking a look at GTK+ and GLib I'm convinced it isn't such a bad idea after all.

[This](https://www.cs.rit.edu/~ats/books/ooc.pdf) book is something I've seen before but never read. In this repo I plan to go through the book and distill the ideas into individual folders with README.md's and example code. Each folder should be self sustaining with its own makefile.

I'm doing this all from a linux machine, so my makefile will use gcc and some things may not translate well to a windows environment.

---

## Sections
For now I'll plan on making a section per chapter. After I finish the book I'll likely make a cheat sheet for myself that will be 3-5 pages of condensed material.
### Chapter 1 - Abstract Data Types — Information Hiding
This section shows how information hiding through **abstract data types** helps to compartmentalize programming. This allows for complete overhauls to the abstract data type without touching the code that uses it.

### Chapter 2 - Dynamic Linkage — Generic Functions
This section shows how you can use **dynamic linkage** to further abstract functionality.

We create a few generic functions and then use **dynamic linkage** to redefine where those functions take us and what they do. This allows us to create class constructors and destructors, as well as other class methods.

### Chapter 3 - Programming Savvy — Arithmetic Expressions
This section takes what you learn in Chapters 1 and 2 and extends it just about as far as it will go. 

You create an arithmetic parser that generates a binary tree of objects and is able to output any of the three following formats
1. Their actual numerical evaluation
2. Their postfix notation equivalent
3. Their infix notation equivalent

All of this is done using **abstract objects** and **dynamic linkage**.

![example image](http://i.imgur.com/4RoGZ0c.png)

---

Coming soon

### Chapter 4 - Inheritance — Code Reuse and Refinement
### Chapter 5 - Programming Savvy — Symbol Table
### Chapter 6 - Class Hierarchy — Maintainability
### Chapter 7 - The ooc Preprocessor — Enforcing a Coding Standard
### Chapter 8 - Dynamic Type Checking — Defensive Programming
### Chapter 9 - Static Construction — Self-Organization
### Chapter 10 - Delegates — Callback Functions
### Chapter 11 - Class Methods — Plugging Memory Leaks
### Chapter 12 - Persistent Objects — Storing and Loading Data Structures
### Chapter 13 - Exceptions — Disciplined Error Recovery
### Chapter 14 - Forwarding Messages — A GUI Calculator
