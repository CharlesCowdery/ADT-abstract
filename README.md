This is a project built for a final assignment in my CS3003 course

I have selected option one, ADT in imperative languages.
I decided that since C++ is specifically an extension of C, that I'd like to get a hang of whats going on under the hood. This means implementing/emulating C++ features in C. 
I chose to implement:
* Templating
* Strings
* Inheritance
With it cumulating in working exceptions. 

For my general strategy, objects were handled using structs, with an associated functions to construct them, that share a name (shout out to the struct keyword delineating objects and functions). They are assigned a few member variables pointing at their functions. This is both for convenience, and so I can apply pseudo-inheritance by nesting constructors.

To handle abstract types/templating, I ended up figuring out two solutions.
* The nicer, more friendly implementation was using void* as an intermediary in arguments. Structures would be given the width of their relevant type, and then would copy from a passed void* to data into their buffers. This worked, but is annoying to use.
* Later on I came up with using preprocessor define functions to generate a typed verion of a block of code. This almost perfectly emulates C++ templating, with typed returns and arguments. It has ALOT of problems though
	* Past a certain text length #define just cuts off the text, limiting code length, and even the length of the passed type.
	* You have to \ every line
	* absolutely impossible to debug
	* types with spaces in them (unsigned int) are a nightmare
	* ew >:(
* I only implemented preprocessor templating for Vector and Pair, Vector as a proof of concept, Pair out of sheer necessity. Pair is only practical if implemented this way, as otherwise the constant width of the type requires you use two void* instead of actual data.
* For vector, the length issue was addressed in a few ways:
	- Inheriting predefined code as possible from the untyped vector, applying a small wrapper where necessary.
	- typedefing to shorten long struct names. Also, abbreviating method names
	- With more patience, you can probable nest Define commands for each method

To be continued

I've decided to implement my exception function with two arguments, an exception literal (unsigned long), and message (string). The exception literal represents exception type. The name associated with the literal will be looked up in a tree, and then printed along with the message, before either exit or an abort signal is raised.

Basic Architecture:
* Vector: accepts arbitrary datatype and act like a regular c++ vector
* String: Manages a char string. Mainly a wrapper around vector, with some minor helper functions
* LongTree: Represents a fixed height binary tree, implemented with left-right pointer nodes, into ADT leaves.
