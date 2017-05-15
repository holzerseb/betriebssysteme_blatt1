Exercise 1 - Notes:
_____________________

See btree.c




Exercise 2 - Notes:
_____________________

###
When would you use a sorted binary tree over a linked list?
###

When there is sortable data, then we can reduce the required time to find a specific
data inside the sorted binary tree. For a balanced binary tree, we can even find
the data in (nearly) half the time.


###
What would you change if the tree had to support not just integers, but any type of data?
###

Probably would switch from C to C++, but if that's not an option, you can either use:
Unions:
	whereas you can declare a field to store any data, that is defined - but the field can
	always only keep one value:
	union Data {
		int i;
		float f;
		whateverStruct ws;
	}
	Data can only have either an int, float or the struct - but you can choose at any time,
	which of the types you want to have.
	
If Unions are not an option as well, your best bet will be an void pointer:
void* pointer;
This pointer can point to whatever data you want it to point to. But imo this is not really
typesafe nor readable and you must know at any time, what exactly there is under the pointer.


###
Why do we use modules instead of putting everything in a single file?
###

yeah.. why would you put all in one file?

This way we can structure our code, make it more maintainable, readable, and most of all: Re-Usable.
We can use the btree for any other project we want, not just in combo with btree_test


###
Is the development cycle (recompiling, ...) faster when using modules? Explain
###

Sure it is. As mentioned above: It's more readable, maintainable, wen can track our bugs easier, when we
know it's just in a single module and we don't have to compile the whole thing, but just a single module.

Also, the other modules rely on the fact, that they just use another module - making it easier to use a module
while we don't have to bother how we implement it later - at the moment, we just wanna use it.


###
What is a header guard and why is it needed?
###

Header Guards ensure that a header file is only included once (you could also use pragma once, if your compiler
does support pragma, while header guards always work)
So if we would have 2 source files both including a single header file, the header guard ensures, that across both
files, the header will not be included twice (resulting in double-declaration, which is not allowed)


###
Why are btree and btree_node not defined in the header file? what are the implications?
###

They are not defined, because the structures are actually private. Nobody needs to know, how the structs
are defined - they just wanna use it. That also means, that outside the btree.c, nobody can access the
members of the header file, so we would have to declare methods in the header file to access them.
That's called data encapsulation and we should try to achieve that.


###
Explain the const in the parameter list of btree_print, is it requireed?
###

It isn't required - but it's a "should-be". the const in the parameter is like a contract,
whereas the btree_print function says: "I will not modify the node you hand to me - why would
I even? I just wanna print it."
It's a better coding style if it's there, the user will know at the moment he uses the function
that my node will not be touched.

You could even cast the node inside the function and modify it - but that's really bad behaviour.


###
Explain Memory Leaks, why are they bad?
###

Memory Leaks are simply allocated/reserved memory, that is neither used nor can be free'd.
They exist, because we loose the reference to allocated memory for example, or just don't free
memory that is not gonna be used anyway.

They are bad, because we reserve memory, that we aren't going to use....


###
What is the reason behind writing everything in english?
###

because english is a common language, most developers speak and this ensures, that everybody can
read the code.
Hell, imagine reading a source file written in spanish? (and you dont speak spanish)


###
Why should you use static for non-exported functions?
###

If a function is non-exported, we wanna use it only in the current file (or tanslation-unit [everything that goes into the *.o file]).
This is what a static method is for - we can only use it inside the current file/translation unit.

Outside, you cannot call the function - actually it doesn't exist for the outside, so you can even declare it and whatever


###
Why should we comment our source code? Is it always need? What should the comment state? what is self-documenting code?
###

Because when we write code, we often do that without really being aware, or somedays we develop having a specific information in mind,
that is simply lost after time or someone else doesn't know.

so comments aren't needed always - proper code can be pretty self-explaining, but comments should be used to document the purpose of
functions/areas, maybe even to specify stuff.

Comments should be used to describe the circumstances, why this method was created, what was the aim - but not "this is for loop, it loops"
Comments for exported functions can also be used to describe what you can feed into the function, what you'll get back, how to use the function

Self-documenting code doesn't exist. Code never states, what the developer was aiming for - it just tells you, what it will currently do.
Self-Documenting != Readable


###
Why should the module not output debug messages?
###

Because if we use the module somewhere else, we can't know how it will be used. Maybe even without a console, so the messages go nowhere.

But I have to disagree with the fact, that this should be common-rules - there are cases where outputting debug-messages is appropriate.
For example to create log files


###
Why and when should you use assert?
###

Assert is really just a debugging tool - you can "disable" it with the -DNDEBUG flag
So, with that knowledge, it's safe to say, that we should use assert only for debugging, to abort a process,
if we cannot continue with valid/usefull data.
So, to check for situation, that "shouldn't have happened"

Note, since it is a real debug-tool, its performance is terrible, but it can tell you useful information

But it's important to state, this is not error-handling!!
As said, its performance is terrible, its a debug-tool and it aborts a whole process.

Someone on stackoverflow once had a really interesting explanation:
Imagine a formula one driver at the start of the race:
	If he doesn't have a helmet, security belt and things, he wants it to be handled before the races starts
	--> thats error handling, someone comes to his car and gives him a helmet for example.
	On the other side, if the race is already going on, he wants the halt the whole race/process, because
	of the risk of damage to him.
	--> there we should use a assert


###
What are the benefits of using make over calling the compiler by hand?
###

It's way easier, the makefile also states, how all the files are in relation to each other and
if someone who didn't contribute to the whole development process wants to build it, he just has to type: "make"


###
Imagin it was your job to design the interface for the btree module (writing btree.h)
What would you have done differently and why?
###

Guess this question can be discussed to an unlimited extend, because everybody does everything differently,
so I just cap it down to 2 main points, which "annoyed" me during the development process:

- An overloaded function for btree_create() -->  "btree* btree_create(btree_node* root);"
	This would have been really useful if you want to create a subtree of a tree, when you already
	know your root. Could have been implemented with two lines as well

- CamelCase: I like CamelCase more over those "_", because this way you preserve yourself a character, which
	you can use to "split" a variable name, like "btreeNode_smallestChild", rather than btree_node_smallest_child
	In the first, it's clear, the smallestChild of btreeNode, rather than the child of the smallestBtreeNode
	But thats just taste








