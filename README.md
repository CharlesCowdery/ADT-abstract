If youre reading this, please come back in a few hours. I got overly ambitious, and then ran into some utterly awful bugs, so I am running super late. The bugs and squashed and the code is essentially done, but needs the demo completed. It should be done sometime saturday.

This is a project built for a final assignment in my CS3003 course

I have selected option one, ADT in imperative languages.

Overall Idea
I've written a LOT of C++ code in my time, and I look forward to writing much more into the future, but I've never really gotten in the gears of how the language works. I ought to finally take a look at the language its an extension of, and how that language was extended to create C++. Thus, I decided to go about implementing/emulating some fundamental C++ features in C. 
Underneath every OOP language is an imperative base, so to understand how that translates into OOP, I chose to implement:
* Templating / generics
* Inheritance
* Strings
* Exceptions

The exceptions are the main goal here, as not having them _immediately_ pissed me off. I do not enjoy passing error states as NULL. Its too annoying to debug. I'm going to need all the planned features to implement them.

(note, I only just now while writing this remembered assert exists. Oops).

So the planned layout for the exceptions is:
* Each exception is represented by a 64-bit integer literal (done via preprocessor)
* Each of these exceptions will be stored in a table, corresponding them to a base message
* When an exception is "thrown" by calling _throw\_exception_, an error message in the form of a string is passed as well as the literal.
* This literal is looked up in the message table, its default message concatenated with the provided error message, displayed, and abort is called.

To do so I needed a few objects to work with:
* A string object in which to store messages, such that they can be concatenated together easily
* A generic vector object upon which the string can be built
* Some form of lookup table for the error messages
	* I settled on a pretty basic fixed height binary tree.

Before we hit on problems, I'm just going to quickly cover how I handled data structures. 
* The base structure of objects is structs
* Since C can distinguish structs and functions via the struct keyword, the constructor for the object is \[Object name]() and new_\[Object name]() for value and ptr respectively
* Associated functions/procedures were declared using the naming scheme \_\_OBJECT_NAME_MEMBER_FUNCTION_function_name(). I absolutely was not going to put up with namespace pollution
* When memory overhead wasn't a worry, I bound associated procedures and functions to the object via function pointers during construction. This is both for convenience and for later pseudo-inheritance.

The first and immediate problem was generics. Since C doesn't provide object templating I had to get a bit creative with my solutions. I came up with two:
* Declaration of the width of the stored type at construction, and then the passing of items via void* in and out of the object.
* Preprocessor word substitution via \#define function directives, generating a custom declaration of the class that uses the type.

The first solution is clean, if a bit inconvenient. Data is passed in via void*, copied into the relevant buffer, and then later during access can simply have its pointer returned. Given that I cannot template the arguments of the functions, data can only be passed by reference, preventing convenient inline function calls, like push_back(4). This solution works in _most_ cases, but fails in certain cases we'll see later.

The second solution is how the C++ compiler actually handles templating (roughly). This though is an abomination. A #define macro is used to substitute in the templated type into the class, creating a custom struct+functions for the type. Its an almost perfect substitute for templating in C++, with some painful caveats.
* These templated objects have a limited number of characters to work with before the preprocessor cuts them off. This severely limits the size of objects. Types with longer names can cause this to happen even if the struct declaration works normally.
* They cannot be easily debugged, at least in visual studio. The macros are not expanded while debugging. I often just manually subbed in the type so intellisense could read the class.
* They work poorly with multiword types, like unsigned int. They must be typedef'd into a single word
* You end up with a typed declaration generator, that must be invoked early in the source code before the templated object is used.
* They are _very_ ugly to look at and work on
What worked well here was to use my previously created void* based generic, redeclare it in the macro with the template type (template type name is merged onto the objects name), and then lightly wrap its existing functions where needed. 

Essentially, the template macro inherits from the base generic class, and then builds on top of it. In vector, I added two typically unassigned function pointers on the generic template for a pass by value version of _push_ and _at_, and then only defined them on the templated version. I also overrided the existing pass by void* functions to pass by the templated type's pointer. 

To be continued
