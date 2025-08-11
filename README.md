video link: [youtube.com/watch?v=Xj3sVVzeLOc&feature=youtu.be]

This is a project built for a final assignment in my CS3003 course

I have selected option one, ADT in imperative languages.

Overall Idea
I've written a LOT of C++ code in my time, and I look forward to writing much more into the future, but I've never really explored how the language works. What I know is that its an extension of C, so I'm curious how that language was extended to create C++. Thus, I decided to go about implementing/emulating some fundamental C++ features in C. 

Underneath every OOP language is an imperative basis (except maybe itanium?) so to understand how that translates into OOP, I chose to implement:
* Templating / generics
* Inheritance
* Strings
* Exceptions

(To context this next part, I had forgotten that assert exists).
As to how I got this list, my goal was originally to just implement vector, but it quickly became to implement exceptions. Not having them _immediately_ pissed me off. The approach I had to take in lieu of them was to indicate function success via (ptr or True) pass vs NULL fail. I found this awful to debug, and I didnt like how it forced most procedures to instead be functions. Anyways, my plans became to work the rest of my planned features into the exception implementation.

So the planned layout for the exceptions is:
* Each exception category is represented by a 64-bit integer literal (done via preprocessor defines)
* Each of these exceptions will be stored in a table, corresponding them to a base message
* When an exception is "thrown" by calling _throw\_exception_, an error message in the form of a string is passed as well as the literal.
* This literal is looked up in the message table, its default message concatenated with the provided error message, displayed, and abort is called.

Not implemented, but an obvious extension of this implementation would be the ability to hook in handlers to catch and handle exceptions. Implementing this would not be hard, but making good use of it would be.

So, to implement exceptions I needed a few objects to work with:
* A string object in which to store messages. My goal is to be able to easily concatenate multiple strings without directly operating on char* memory.
* A generic vector object upon which the string can be built
* Some form of lookup table for the error messages. Given the lookup is a 64 bit int, I chose to use a fixed height binary tree. I wanted to handle neither hashing nor tree rebalancing.

Before we hit on problems, I'm just going to quickly cover how I handled data structures. 
* The base structure of objects is structs
* Since C can distinguish structs and functions via the struct keyword, I'm able to name the constructor function of each object the same as the object. 
* I wanted to avoid namespace pollution, so associated functions/procedures were declared using the naming scheme \_\_\[OBJECT_NAME]\_MEMBER\_FUNCTION_function_name(). 
* When memory overhead wasn't a worry, I bound associated procedures and functions to the object by declaring members that would be assigned function pointers during construction.
	* This design choice ended up being quite useful later on for inheritance.
	* As I understand it, this is pretty analogous to how a vtable in C++ works, and is how virtual functions are implemented.
	* This highlighted to me the reason that python passes self via argument in member functions. C++ hides it, but all member functions are imperative functions in disguise. No matter what, if you want to operate on an object, you you have to pass its pointer.


The first and immediate problem was parameterization. Since C doesn't provide object templating, I had to do things the hard way. I had two solutions:
* The width of a containers type can be passed at construction, and this width be used to initialize memory, and then copy the correct width of memory when copying in objects. Data can be passed in and out via void*.
	* Generally, this is the most flexible solution when templating is not an option. It suffers from introducing a high degree of indirection though, and is unable to pass by value.
* Preprocessor term substitution via \#define macros, enabling the generation of a custom declaration of the class using the type.
	* This is roughly equivalent to how templating works in C++, with some minor caveats.
	* Allows for pass by value returns and arguments, and typing of, for example, the buffer ptr.
	* This has many many implementation problems.
		* The final generated code is limited in length, being cut off at a compiler specific number of characters (~1024-2048 in msvc?).
		* Impossible to debug, as the macros are not expanded while debugging.
		* Works poorly with multi-word types like unsigned int.
		* The templated class generator must be explicitly invoked for a given type. Handling the possibility that this generator may accidentally be invoked again later in source requires additional preprocessor logic.

This length limit introduced some interesting complications and solutions. Because the declaration had to be kept short, as much as possible from the void* based class was reused. Resultingly, I kept the memory structure identical to the void* based class, lightly wrapped the constructor (like Super!), and reused all existing procedures/functions. I was however, able to retype the existing functions, such that the compiler can recognize what types go in and out of the vector.

Now, this retyping alone would reap very few benefits. I had a bit of budget left after core declarations, so I decided to add two additional function fields to the _base_ class for pass by value versions of functions, in this case, push_back() and at(). The base class would leave these undefined, and the templated class would define these functions during construction.

So for all intents and purposes, the templated class is a derived class of the void* class that gives definition to abstract/virtual functions to extend functionality. Pretty neat.

This macro templating is fairly labor intensive, so I only implemented it for vector, and a very small pair class, but the results were remarkable. The syntactic style resulting from this was remarkably close to that of C++, with templated vectors invoked with the type Vector(t), much akin to Vector<t>.

As for String, its a fairly lightweight wrapper around vector, that implements a few operator procedures for string assignment and appendment. I decided to embrace the imperative style for String, and directly call Vector's member functions instead of using the function pointer fields on the object. Anyways, as for the fixed height tree class, it uses void* generics, and is by all accounts unremarkable.

Once I started to create structures of classes, such as a vector of strings, a problem cropped up. These generic functions have no way of automatically accessing a copy constructor or delete function for their data. As I had implemented it, they simply malloc'd and free'd their buffers, naively copying and deleting objects. This is less than ideal. To accomadate this problem, I added functionality to optionally provide a *copy\_function* and _delete\_function_ that could be used instead of malloc/free during object assignment and deletion.

It became abundantly clear these constructor/destructor woes were a broader issue. I found out the hard way that while I _could_ pass many classes by value, the lack of automatic object lifetime management, including appropriate use of destructors, made doing so sketchy. This caused quite the headache, and makes me sorely miss the warm embrace of automatic object lifetime management and proper objects. Between this and pointer arithmetic errors, I spent so many hours debugging memory corruptions. So many.

It took longer than I'd like to admit to get rid of the memory corruptions in String that were causing throw_exception to throw exceptions, but In the end, I was successful. Finally, I could replace my return based error management with the **venerable** throw_exception. 

This project has been pretty revealing about not only how C++ works under the hood, but why it works that way. It was neat to see templating syntax arise out of nowhere with my macros. I focus on program optimization, so I anticipate this knowledge of the imperative basis of C++ will actually be fairly useful. Its a small example, but understanding *why* member functions can be inlined into code, but virtual functions must be handled via a memory lookup into an actual function call, is very useful in diagnosing performance pitfalls.
