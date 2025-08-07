This is a project built for a final assignment in my CS3003 course

I have selected option one, ADT in imperative languages.
I started out constructing a vector struct. I didn't like signalling exceptions with false/NULL on functions, so the metagoal of this project became the impelementation of an exception function. 

I decided to handle abstract types via void*s. Values are passed via void\*, and then memcpy'd into the structure's allocated memory. ADT objects are initialized with the bytewidth of their ADT type as an argument. 

I've decided to implement my exception function with two arguments, an exception literal (unsigned long), and message (string). The exception literal represents exception type. The name associated with the literal will be looked up in a tree, and then printed along with the message, before either exit or an abort signal is raised.

Basic Architecture:
* Vector: accepts arbitrary datatype and act like a regular c++ vector
* String: Manages a char string. Mainly a wrapper around vector, with some minor helper functions
* LongTree: Represents a fixed height binary tree, implemented with left-right pointer nodes, into ADT leaves.
