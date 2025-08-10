// ADT abstract.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
//#include <random>

#define GLUE_HELPER(x, y) x##y
#define GLUE(x, y) GLUE_HELPER(x, y)
#define SUB_PTR(a) 

typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef void* void_ptr;
#define Pair(type_1,type_2) GLUE(GLUE(GLUE(Pair_,type_1),_),type_2) 
#define GENERATE_PAIR_DECLARATION(type_1,type_2)\
struct Pair(type_1,type_2){\
	type_1 first;\
	type_2 second;\
};\

//struct Pair_generic {
//	void* first;
//	void* second;
//};
//
//struct Pair_generic* Pair_generic(unsigned long first_byte_width, unsigned long second_byte_width) {
//	struct Pair_generic* pair = malloc(first_byte_width + second_byte_width);
//	pair->first = pair;
//	pair->second = ((char*)pair->first) + first_byte_width;
//	return pair;
//}


//struct Vector errors;
//void build_error_table() {
//	errors = Vector(sizeof(struct String));
//	errors.push_back(&errors,"Out of Bounds exception");
//}
//
//void error_handler(int signal) {
//
//}
typedef struct String;
struct String String(char* c_str);
struct String* new_String(char str[]); 

#define UNKNOWN_ERROR 0
#define OUT_OF_RANGE 1
#define INVALID_ARGUMENT 2
#define UNDERFLOW 3
#define OVERFLOW  4

struct FixedTree* error_table;
//void (*throw_exception)(uint64_t error_literal, struct String message);
void* throw_exception(uint64_t error_literal, struct String* message);



struct Vector_generic {
	int object_byte_size;
	uint64_t size;
	uint64_t reserved_size;
	uint64_t size_bytes;
	uint64_t reserved_bytes;
	void* data;
	void* end;
	bool (*reserve)(struct Vector_generic* self, uint64_t size) ;
	void* (*push_back_ref)(struct Vector_generic* self, void* item);
	bool (*pop_back)(struct Vector_generic* self);
	void* (*at_ref)(struct Vector_generic* self, uint64_t index);
	bool (*remove)(struct Vector_generic* self, uint64_t starting_index, uint64_t length);
	bool (*delete)(struct Vector_generic* self);
	void (*copy_function)(void* address, void* object);
	void (*delete_function)(void* address);
	void (*at)();
	void (*push_back)();
};

bool __VECTOR_MEMBER_FUNCTION_resize_reserve(struct Vector_generic* vector, uint64_t new_reserved_size_items) {
	if (vector == NULL) return throw_exception(INVALID_ARGUMENT,new_String("pointer passed to resize was null"));
	///SANITY CHECK///

	uint64_t new_reserved_size_bytes = 0;
	if (new_reserved_size_items == -1) {
		new_reserved_size_items = vector->reserved_size << 1; 
		new_reserved_size_bytes = vector->reserved_bytes << 1;
	}
	else {
		new_reserved_size_bytes = new_reserved_size_items * vector->object_byte_size;
	}
	if (new_reserved_size_bytes < vector->size_bytes) {
		return false;
	}

	void* current_memory = vector->data;
	void* new_memory = malloc(new_reserved_size_bytes);
	if (new_memory == NULL) return false;
	if (current_memory != NULL) {
		memcpy(new_memory, current_memory, vector->size_bytes);
		free(current_memory);
	}
	vector->reserved_bytes = new_reserved_size_bytes;
	vector->reserved_size = new_reserved_size_items;
	vector->data = new_memory;
	vector->end = (unsigned char*) vector->data + vector->size_bytes;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_push_back(struct Vector_generic* vector, void* obj) {
	if (vector == NULL)			return throw_exception(INVALID_ARGUMENT,new_String("vector pointer was null"));
	if (vector->data == NULL)	return throw_exception(INVALID_ARGUMENT,new_String("vector has been deleted"));
	if (obj == NULL)			return throw_exception(INVALID_ARGUMENT,new_String("passed ptr to object is null"));
	///SANITY CHECK///

	uint64_t post_used_bytes = vector->size_bytes + vector->object_byte_size;
	if (post_used_bytes > vector->reserved_bytes) {
		bool resize_success = __VECTOR_MEMBER_FUNCTION_resize_reserve(vector, -1);
		if (!resize_success) return NULL;
	}
	void* item_position = vector->end;
	if (vector->copy_function != NULL) {
		vector->copy_function(item_position, obj);
	}
	else {
		memcpy(item_position, obj, vector->object_byte_size);
	}
	vector->size_bytes = post_used_bytes;
	vector->size += 1;
	vector->end = ((unsigned char*) vector->end) + vector->object_byte_size;
	return item_position;
}

bool __VECTOR_MEMBER_FUNCTION_pop_back(struct Vector_generic* vector) {
	if (vector == NULL)				return throw_exception(INVALID_ARGUMENT, new_String("vector pointer was null"));
	if (vector->data == NULL)		return throw_exception(INVALID_ARGUMENT, new_String("vector has been deleted"));
	if (vector->size == 0)			return throw_exception(UNDERFLOW, new_String("vector is empty"));
	///SANITY CHECK///

	vector->size -= 1;
	vector->size_bytes -= vector->object_byte_size;
	if (vector->delete_function != NULL) {
		vector->delete_function(vector->end);
	}
	vector->end = (unsigned char*)vector->end - vector->object_byte_size;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_at(struct Vector_generic* vector, uint64_t index) {
	if (vector == NULL)							return throw_exception(INVALID_ARGUMENT, new_String("vector pointer was null"));
	if (vector->data == NULL)					return throw_exception(INVALID_ARGUMENT, new_String("vector has been deleted"));
	if (index<0 || index >= vector->size)		return throw_exception(OUT_OF_RANGE, new_String("vector indexing error, out of range"));
	///SANITY CHECK///

	return ((unsigned char*) vector->data) + index * vector->object_byte_size;
}

void* __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(struct Vector_generic* vector, uint64_t index) {
	return ((unsigned char*)vector->data) + index * vector->object_byte_size;
}

bool __VECTOR_MEMBER_FUNCTION_delete(struct Vector_generic* vector) {
	if (vector == NULL)				return throw_exception(INVALID_ARGUMENT, new_String("vector pointer was null"));
	if (vector->data == NULL)		return throw_exception(INVALID_ARGUMENT, new_String("vector has been deleted"));
	///SANITY CHECK///

	if (vector->delete_function != NULL) {
		for (int i = 0; i < vector->size; i++) {
			vector->delete_function(vector->at_ref(vector, i));
		}
	}

	free(vector->data);
	free(vector);
}

bool __VECTOR_MEMBER_FUNCTION_delete_partial(struct Vector_generic* vector) {
	if (vector == NULL)				return throw_exception(INVALID_ARGUMENT, new_String("vector pointer was null"));
	if (vector->data == NULL)		return throw_exception(INVALID_ARGUMENT, new_String("vector has been deleted"));
	///SANITY CHECK///

	if (vector->delete_function != NULL) {
		for (int i = 0; i < vector->size; i++) {
			vector->delete_function(vector->at_ref(vector, i));
		}
	}

	free(vector->data);
}

bool __VECTOR_MEMBER_FUNCTION_remove(struct Vector_generic* vector, uint64_t starting_index, uint64_t length) {
	if (vector == NULL)							return throw_exception(INVALID_ARGUMENT, new_String("vector pointer was null"));
	if (vector->data == NULL)					return throw_exception(INVALID_ARGUMENT, new_String("vector has been deleted"));
	if (length < 0 || starting_index < 0)		return throw_exception(OUT_OF_RANGE, new_String("vector indexing error, out of range"));
	if (starting_index + length > vector->size)	return throw_exception(OUT_OF_RANGE, new_String("cannot remove more elements than exist from starting index to end"));
	///SANITY CHECK///

	uint64_t ending_index = starting_index + length;
	void* start = __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, starting_index);
	void* end =	  __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, ending_index); //this places the pointer on the element AFTER the final element removed

	uint64_t trailing_length = vector->size - ending_index;
	uint64_t removed_bytes = length * vector->object_byte_size;

	memcpy(start, end, trailing_length*vector->object_byte_size);
	vector->size -= length;
	vector->size_bytes -= removed_bytes;
	vector->end = (unsigned char*)vector->end - removed_bytes;

	return true;
}

bool __VECTOR_MEMBER_FUNCTION_splice(struct Vector_generic* vector, struct Vector_generic* other, uint64_t destination_index, uint64_t source_index, uint64_t length) {
	if (vector == NULL)														return throw_exception(INVALID_ARGUMENT, new_String("vector pointer was null"));
	if (vector->data == NULL)												return throw_exception(INVALID_ARGUMENT, new_String("vector has been deleted"));
	if (vector->object_byte_size != other->object_byte_size)				return throw_exception(INVALID_ARGUMENT, new_String("cannot splice items of different width together"));
	if (destination_index < 0 || destination_index > vector->size)			return throw_exception(OUT_OF_RANGE, new_String("vector indexing error, out of range"));
	if (source_index < 0 || length < 0)										return throw_exception(OUT_OF_RANGE, new_String("vector indexing error, out of range"));
	if (source_index + length > other->size)								return throw_exception(OUT_OF_RANGE, new_String("vector indexing error, out of range"));

	///SANITY CHECK///

	uint64_t destination_final_length = vector->size + length;
	while (destination_final_length > vector->reserved_size) {
		vector->reserve(vector, -1);
	}
	void* destination_splice_start	= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index);
	void* destination_splice_end	= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index + length);
	void* source_splice_start		= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(other, source_index);



	uint64_t length_bytes = length * vector->object_byte_size;
	uint64_t trailing_bytes = (unsigned char*)vector->end - destination_splice_start;
	if (vector->copy_function != NULL) {
		void* source_address = destination_splice_start;
		void* destination_address = destination_splice_end;
		while (true) {
			vector->copy_function(destination_address, source_address);
			vector->delete_function(source_address);
			if (source_address == destination_splice_end) break;
			((char*)source_address) += vector->object_byte_size;
			((char*)destination_address) += vector->object_byte_size;
		}
		source_address = source_splice_start;
		destination_address = destination_splice_start;
		while (true) {
			vector->copy_function(destination_address, source_address);
			vector->delete_function(source_address);
			if (source_address == destination_splice_start) break;
			((char*)source_address) += vector->object_byte_size;
			((char*)destination_address) += vector->object_byte_size;
		}
	}
	else {
		memcpy(destination_splice_end, destination_splice_start, trailing_bytes);
		memcpy(destination_splice_start, source_splice_start, length_bytes);
	}

	vector->size += length;
	vector->size_bytes += length_bytes;

	return true;
}

bool __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(struct Vector_generic* vector, void* other, uint64_t destination_index, uint64_t source_index, uint64_t length) {
	///SANITY CHECK///

	uint64_t destination_final_length = vector->size + length;
	while (destination_final_length > vector->reserved_size) {
		vector->reserve(vector, -1);
	}
	void* destination_splice_start =	__VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index);
	void* destination_splice_end =		__VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index + length);
	void* source_splice_start =			(unsigned char*) other + source_index*vector->object_byte_size;

	uint64_t length_bytes = length * vector->object_byte_size;
	uint64_t trailing_bytes = (unsigned char*) vector->end - destination_splice_start;

	if (vector->copy_function != NULL) {
		void* source_address = destination_splice_start;
		void* destination_address = destination_splice_end;
		while (true) {
			vector->copy_function(destination_address, source_address);
			vector->delete_function(source_address);
			if (source_address == destination_splice_end) break;
			((char*)source_address) += vector->object_byte_size;
			((char*)destination_address) += vector->object_byte_size;
		}
		source_address = source_splice_start;
		destination_address = destination_splice_start;
		while (true) {
			vector->copy_function(destination_address, source_address);
			vector->delete_function(source_address);
			if (source_address == destination_splice_start) break;
			((char*)source_address) += vector->object_byte_size;
			((char*)destination_address) += vector->object_byte_size;
		}
	}
	else {
		memcpy(destination_splice_end, destination_splice_start, trailing_bytes);
		memcpy(destination_splice_start, source_splice_start, length_bytes);
	}

	vector->size += length;
	(char*)vector->end += length;
	vector->size_bytes += length_bytes;

	return true;
}

struct Vector_generic* new_Vector_generic(int object_byte_size) {
	struct Vector_generic* vector = malloc(sizeof(struct Vector_generic));
	vector->data = NULL;
	vector->object_byte_size = object_byte_size;
	vector->reserved_bytes = 0;
	vector->reserved_size = 0;
	vector->size_bytes = 0;
	vector->size = 0;
	vector->reserve = &__VECTOR_MEMBER_FUNCTION_resize_reserve;
	vector->push_back_ref = &__VECTOR_MEMBER_FUNCTION_push_back;
	vector->pop_back = &__VECTOR_MEMBER_FUNCTION_pop_back;
	vector->at_ref = &__VECTOR_MEMBER_FUNCTION_at;
	vector->delete = &__VECTOR_MEMBER_FUNCTION_delete;
	vector->remove = &__VECTOR_MEMBER_FUNCTION_remove;
	vector->copy_function = NULL;
	vector->delete_function = NULL;
	vector->at = NULL;
	vector->push_back = NULL;
	vector->reserve(vector, 1);
	return vector;
}

struct Vector_generic Vector_generic(int object_byte_size) {
	struct Vector_generic* vector = new_Vector_generic(object_byte_size);
	struct Vector_generic return_object = *vector;
	free(vector);
	return return_object;
}

struct Vector_generic* __VECTOR_MEMBER_FUNCTION_deep_copy(void* destination,struct Vector_generic* vector) {
	if (vector == NULL) return throw_exception(INVALID_ARGUMENT, new_String("Passed vector is nullptr"));
	if (destination == NULL) return throw_exception(INVALID_ARGUMENT, new_String("Destination of copy cannot be nullptr"));
	memcpy(destination, vector, sizeof(struct Vector_generic));
	struct Vector_generic* new_vec = destination;
	new_vec->data = NULL;
	new_vec->reserve(new_vec,vector->size);
	__VECTOR_MEMBER_FUNCTION_splice(new_vec, vector, 0, 0, vector->size);
	return new_vec;
}


#define Vector(t) GLUE(Vector_,t)
#define new_Vector(t) GLUE(new_Vector_,t)

typedef struct Vector_generic __VG;

#define GENERATE_TYPED_VECTOR_DECLARATION(suffix,t) \
struct Vector(suffix) {\
int object_byte_size;\
uint64_t size;\
uint64_t reserved_size;\
uint64_t size_bytes;\
uint64_t reserved_bytes;\
t* data;\
t* end;\
bool (*reserve)(__VG* self, uint64_t size);\
t* (*push_back_ref)(__VG* self, t* item);\
bool (*pop_back)(__VG* self);\
t* (*at_ref)(__VG* self, uint64_t index);\
bool (*remove)(__VG* self, uint64_t starting_index, uint64_t length);\
bool (*delete)(__VG* self);\
void (*copy_function)(void* address, void* object);\
void (*delete_function)(void* address);\
t (*at)(__VG* self, uint64_t index);\
t* (*push_back)(__VG* self, t item);\
};\
t GLUE(__AVF_at_,suffix)(__VG* s, uint64_t i){\
	return *(t*)(s->at_ref(s,i));\
}\
t* GLUE(__AVF_push_,suffix)(__VG* s, t i){\
	return s->push_back_ref(s,&i);\
}\
struct Vector(suffix)* new_Vector(suffix)() {\
	struct Vector_generic* generic_vector = new_Vector_generic(sizeof(t));\
	return generic_vector;\
}\
struct Vector(suffix) Vector(suffix)(){\
	struct Vector(suffix)* vec_ptr = new_Vector(suffix)();\
	vec_ptr->at = &GLUE(__AVF_at_,suffix);\
	vec_ptr->push_back = &GLUE(__AVF_push_, suffix);\
	struct Vector(suffix) solid_vec = *vec_ptr;\
	free(vec_ptr);\
	return solid_vec;\
}

GENERATE_TYPED_VECTOR_DECLARATION(int, int)
GENERATE_TYPED_VECTOR_DECLARATION(char, char)

struct String {
	struct Vector(char) vector;
	uint64_t(*size)(struct String* self);
	char* (*c_str)(struct String* self);
	bool  (*assign)(struct String* self, char c_str[]);
	bool  (*assign_dynamic)(struct String* self, char* c_str, uint64_t length);
	bool  (*delete)(struct String* self);
	struct String* (*append)(struct String* self, char* c_str);
};

uint64_t __STRING_MEMBER_FUNCTION_size(struct String* string){
	return string->vector.size-1;
}

char* __STRING_MEMBER_FUNCTION_c_str(struct String* string){
	return string->vector.data;
}

bool __STRING_MEMBER_FUNCTION_assign(struct String* string, char c_str[]) {
	if (c_str == NULL) return throw_exception(INVALID_ARGUMENT, new_String("cannot copy string from nullptr"));
	string->vector.remove(&string->vector, 0, string->vector.size);
	int length = strlen(c_str);
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, 0, 0, length);
	bool push = string->vector.push_back_ref(&string->vector,"\0"); 
	return splice_return && push;
}

bool __STRING_MEMBER_FUNCTION_append(struct String* string, char* c_str) {
	string->vector.pop_back(&string->vector);
	int length = strlen(c_str);
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, string->vector.size-1, 0, length);
	bool push = string->vector.push_back_ref(&string->vector, "\0");
}

bool __STRING_MEMBER_FUNCTION_delete(struct String* self) {
	__VECTOR_MEMBER_FUNCTION_delete_partial(& self->vector);
	free(self);
}

struct String* __STRING_MEMBER_FUNCTION_deep_copy(void* destination, struct String* string) {
	if (string == NULL) return throw_exception(INVALID_ARGUMENT, new_String("Passed vector is nullptr"));
	if (destination == NULL) return throw_exception(INVALID_ARGUMENT, new_String("Destination of copy cannot be nullptr"));
	memcpy(destination, string, sizeof(struct Vector_generic));
	struct String* new_string = destination;
	__VECTOR_MEMBER_FUNCTION_deep_copy(&new_string->vector, &string->vector);
	return destination;
}

struct String* new_String(char str[]) {
	struct String* string = malloc(sizeof(struct String));
	string->vector = Vector(char)(sizeof(char));
	string->size = &__STRING_MEMBER_FUNCTION_size;
	string->c_str = &__STRING_MEMBER_FUNCTION_c_str;
	string->assign = &__STRING_MEMBER_FUNCTION_assign;
	string->delete = &__STRING_MEMBER_FUNCTION_delete;
	string->append = &__STRING_MEMBER_FUNCTION_append;
	string->assign(string, str);
	return string;
}


struct String String(char* str) {
	struct String* string = new_String(str);
	struct String return_object = *string;
	//printf("%i",return_object.vector.size)
	free(string);
	return return_object;
}

struct FixedTreeNode {
	struct FixedTreeNode* left;
	struct FixedTreeNode* right;
};

bool __FIXED_TREE_NODE_MEMBER_FUNCTION_delete(struct FixedTreeNode* node, int depth, void (*deleter)(void*obj) ) {
	if (node == NULL) return false;
	if (depth != 63) {
		__FIXED_TREE_NODE_MEMBER_FUNCTION_delete(node->left, depth+1, deleter);
		__FIXED_TREE_NODE_MEMBER_FUNCTION_delete(node->right, depth+1, deleter);
	} 
	if (depth == 63) {
		if (node->left != NULL) {
			deleter(node->left);
		}
		if (node->right != NULL) {
			deleter(node->right);
		}
	}
	free(node);
	return true;
}

struct FixedTreeNode* new_FixedTreeNode() {
	struct FixedTreeNode* node = malloc(sizeof(struct FixedTreeNode));
	node->left = NULL;
	node->right = NULL;
	return node;
}

struct FixedTree {
	struct FixedTreeNode* head;
	uint64_t object_byte_width;
	bool (*insert)(struct FixedTree* self, uint64_t id, void* item);
	bool (*remove)(struct FixedTree* self, uint64_t id);
	void* (*find)(struct FixedTree* self, uint64_t id);
	struct Vector(Pair(uint64_t, void_ptr)) (*get_items)(struct FixedTree* self);
	bool (*__designated_copy_function)(void* object_ptr, void* destination);
	bool (*__designated_delete_function)(void* object_ptr);
};

//note the final return case of return node actually returns a pointer to the object at the end, not a node.
void** __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(struct FixedTreeNode** node, uint64_t navkey, uint64_t* bitmask) {
	if (*node == NULL) return node;
	if (*bitmask == 0) {
		return node;
	}
	bool go_left = (navkey & *bitmask) == 0;
	*bitmask = *bitmask << 1;
	if (go_left) {
		return __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(&((*node)->left), navkey, bitmask);
	}
	else {
		return __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(&((*node)->right), navkey, bitmask);
	}
}

void* __FIXED_TREE_MEMBER_FUNCTION_find(struct FixedTree* tree, uint64_t navkey) {
	uint64_t bitmask = 1;
	void** end_node_ptr = __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(&tree->head, navkey, &bitmask);
	return *end_node_ptr;
}

bool __FIXED_TREE_MEMBER_FUNCTION_insert(struct FixedTree* tree, uint64_t navkey, void* insert_object) {
	uint64_t bitmask = 1;
	void** end_node_ptr = __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(&tree->head, navkey, &bitmask);
	//end_node_ptr points at the ptr to the end of the tree. If an item already exists there, it will not be NULL.
	//in that case, bitmask is 0.
	//otherwise, we need to create a new node, and set end_node_ptr to point at that node. We then want to navigate down that node and repeat.
	while (bitmask != 0) {
		*end_node_ptr = new_FixedTreeNode();
		end_node_ptr = __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(end_node_ptr, navkey, &bitmask);
	}
	if (*end_node_ptr != NULL) return false;
	*end_node_ptr = malloc(tree->object_byte_width);
	if (tree->__designated_copy_function != NULL) {
		return tree->__designated_copy_function(insert_object, *end_node_ptr);
	}
	else {
		memcpy(*end_node_ptr, insert_object, tree->object_byte_width);
		return true;
	}
}

bool __FIXED_TREE_MEMBER_FUNCTION_remove_object(struct FixedTree* tree, uint64_t id) {
	struct FixedTreeNode* chain[64];
	int index = 0;
	uint64_t bitmask = 1;
	struct FixedTreeNode** current_pointer = &tree->head;
	while (bitmask != 0) {
		chain[index] = current_pointer;
		index++;
		bool go_left = (id & bitmask) == 0;
		bitmask = bitmask << 1;
		if(go_left){
			current_pointer = &((*current_pointer)->left);
		}
		else {
			current_pointer = &((*current_pointer)->right);
		}
		if (current_pointer == NULL) return false;
	}
	if (tree->__designated_delete_function != NULL) {
		tree->__designated_delete_function(*current_pointer);
		bool go_left = (id & 1ull<<63) == 0;
		if (go_left) {
			chain[63]->left = NULL;
		}
		else {
			chain[63]->right = NULL;
		}
		//*current_pointer = N;
	}
	index -= 1;
	struct FixedTreeNode* last = chain[index];
	while (index > 0) {
		if (chain[index]->left != NULL && chain[index]->right != NULL) {
			index++;
			break;
		}
		index--;
	}
	if (index == 64) return;
	__FIXED_TREE_NODE_MEMBER_FUNCTION_delete(chain[index], index, tree->__designated_delete_function);
	if (chain[index - 1]->left == chain[index]) {
		chain[index - 1]->left = NULL;
	}
	else {
		chain[index - 1]->right = NULL;
	}
	return true;
}

GENERATE_PAIR_DECLARATION(uint64_t, void_ptr)
GENERATE_TYPED_VECTOR_DECLARATION(Pair(uint64_t,void_ptr),struct Pair(uint64_t,void_ptr))

struct Vector(Pair(uint64_t,void_ptr)) __FIXED_TREE_MEMBER_FUNCTION_get_items(struct FixedTree* tree) {
	struct Vector(Pair(uint64_t, void_ptr)) output = Vector(Pair(uint64_t, void_ptr))();

	struct FixedTreeNode* chain[65];
	char state_chain[65];

	chain[0] = tree->head;
	state_chain[0] = 0;

	int i = 0;
	struct Pair(uint64_t, void_ptr) holder = {0,0};
	uint64_t bitmask = 1;
	uint64_t state = 0;
	while (i > -1) {
		if (i == 64) {
			holder.first = state;
			holder.second = chain[i];
			output.push_back_ref(&output, &holder);
			i--;
			continue;
		}
		if (state_chain[i] == 0) {
			state = state & ~(bitmask << i);
			state_chain[i] = 1;
			if (chain[i]->left != NULL) {
				state_chain[i + 1] = 0;
				chain[i + 1] = chain[i]->left;
				i++;
			}
			continue;
		}
		if (state_chain[i] == 1) {
			state = state | (bitmask << i);
			
			state_chain[i] = 2;
			if (chain[i]->right != NULL) {
				state_chain[i + 1] = 0;
				chain[i + 1] = chain[i]->right;
				i++;
			}
			continue;
		}
		if (state_chain[i] == 2) i--;
	}
	return output;
}




struct FixedTree* new_FixedTree(uint64_t object_byte_width, bool (*copy_function)(void* object_ptr, void* destination), bool(*delete_function)(void* object_ptr)) {
	struct FixedTree* tree = malloc(sizeof(struct FixedTree));
	tree->object_byte_width = object_byte_width;
	tree->__designated_copy_function = copy_function;
	tree->__designated_delete_function = delete_function;
	tree->head = new_FixedTreeNode();
	tree->find = &__FIXED_TREE_MEMBER_FUNCTION_find;
	tree->insert = &__FIXED_TREE_MEMBER_FUNCTION_insert;
	tree->remove = &__FIXED_TREE_MEMBER_FUNCTION_remove_object;
	tree->get_items = &__FIXED_TREE_MEMBER_FUNCTION_get_items;
	return tree;
}

bool abort_on_exception = true;

//Takes an error literal, and message as an argument, and prints an error message.
//If abort on exception is set to true, abort is called.
//The message passed as argument will be freed.
void* throw_exception(uint64_t error_literal, struct String* message) {

	struct String* error_message_base = error_table->find(error_table, error_literal);
	if (error_message_base == NULL) throw_exception(UNKNOWN_ERROR, message);

	struct String final_message = String("\n\nException!: ");
	final_message.append(&final_message, error_message_base->c_str(error_message_base));
	final_message.append(&final_message, ". ");
	final_message.append(&final_message, message->c_str(message));
	final_message.append(&final_message, "\n\n");
	printf(final_message.c_str(&final_message));

	__VECTOR_MEMBER_FUNCTION_delete_partial(&final_message.vector);
	__STRING_MEMBER_FUNCTION_delete(message);

	if (abort_on_exception) abort();
	else exit(1);
	return false;
}

void construct_error_table() {
	error_table = new_FixedTree(sizeof(struct String), NULL, &__STRING_MEMBER_FUNCTION_delete);
	error_table->insert(error_table, UNKNOWN_ERROR, new_String("Unknown error"));
	error_table->insert(error_table, OUT_OF_RANGE, new_String("Out of range"));
	error_table->insert(error_table, UNDERFLOW, new_String("Underflow error"));
	error_table->insert(error_table, OVERFLOW, new_String("Overflow error"));
	error_table->insert(error_table, INVALID_ARGUMENT, new_String("Invalid argument"));
}

//void menu_function(struct Vector(struct_string)* options, struct Vector_generic* funcs) {
//	while (true) {
//		for (int i = 0; i < options.size; i++) {
//			struct String* message = options.at_ref(&options,i);
//			printf(message->c_str(message));
//		}
//	}
//}


typedef struct string struct_string;
GENERATE_TYPED_VECTOR_DECLARATION(struct_string,struct String);

int main()
{	
	srand(time(NULL));
	construct_error_table();
	abort_on_exception = false;

	int add_num_ints = 10;
	int read_num_ints = 11;
	int add_num_strings = 10;
	int read_num_strings = 10;

	int demo_case = 0;


	if (demo_case == 0) {
		struct Vector(int) int_vec_1 = Vector(int)();
		struct Vector_generic int_vec_2 = Vector_generic(sizeof(int));

		//error_table->remove(error_table, OUT_OF_RANGE);
		//error_table->insert(error_table, OUT_OF_RANGE, new_String("BRAND SPANKING NEW ERROR"));


		printf("Adding %i ints to list: \n\tadding: ", add_num_ints);
		for (int i = 0; i < add_num_ints; i++) {
			int r = rand() % 10;
			printf("%i, ", r);
			int_vec_2.push_back_ref(&int_vec_2, &r);
		}
		printf("\n");
		printf("Reading %i ints from list: \n\tlist:   ", read_num_ints);
		for (int i = 0; i < read_num_ints; i++) {
			printf("%i, ", *(int*)int_vec_2.at_ref(&int_vec_2, i));
		}

		printf("Adding %i ints to list: \n\tadding: ", add_num_ints);
		for (int i = 0; i < add_num_ints; i++) {
			int r = rand() % 10;
			printf("%i, ", r);
			int_vec_1.push_back(&int_vec_1, r);
		}
		printf("\n");
		printf("Reading %i ints from list: \n\tlist:   ", read_num_ints);
		for (int i = 0; i < read_num_ints; i++) {
			printf("%i, ", int_vec_1.at(NULL, i));
		}
	}

	if (demo_case == 1) {
		struct Vector(struct_string) string_vec = Vector(struct_string)();
		string_vec.copy_function = &__STRING_MEMBER_FUNCTION_deep_copy;
		string_vec.delete_function = &__STRING_MEMBER_FUNCTION_delete;

		struct String message_holder = String("");
		for (int i = 0; i < 10; i++) {

			char str_buffer[10];
			sprintf_s(str_buffer, 10, "%i\n", i);

			struct String base = String("This is string ");
			struct String id = String(str_buffer);

			message_holder.assign(&message_holder, base.c_str(&base));
			message_holder.append(&message_holder, id.c_str(&id));

			string_vec.push_back(&string_vec, message_holder);
			
			base.delete(&base);
			id.delete(&id);
		}

		//message_holder.delete(&message_holder);

		for (int i = 0; i < 10; i++) {
			printf(__STRING_MEMBER_FUNCTION_c_str(string_vec.at_ref(&string_vec, i)));
		}

	}
	

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
