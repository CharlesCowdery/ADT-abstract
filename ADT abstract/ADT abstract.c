// ADT abstract.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

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

#define UNKNOWN_ERROR 0
#define OUT_OF_RANGE 1
#define INVALID_ARGUMENT 2
#define UNDERFLOW 3
#define OVERFLOW  4

struct FixedTree* error_table;
//void (*throw_exception)(uint64_t error_literal, struct String message);
void* throw_exception(uint64_t error_literal, struct String message);



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
	void (*at)();
	void (*push_back)();
};

//searches for the most significant bit using bitmasks. I just think this algorithm is fun.
uint64_t get_MSB(uint64_t value) {
	uint64_t mask_1 = 0xffffffff00000000; 
	uint64_t mask_2 = 0xffff0000ffff0000;
	uint64_t mask_3 = 0xff00ff00ff00ff00;
	uint64_t mask_4 = 0xf0f0f0f0f0f0f0f0;
	uint64_t mask_5 = 0xcccccccccccccccc;
	uint64_t mask_6 = 0xaaaaaaaaaaaaaaaa;

	uint64_t masked_int = value;
	uint64_t operator_int = masked_int & mask_1;
	uint64_t final_value = 1;
	if (operator_int > 0) {
		masked_int = operator_int;
		final_value = final_value << 32;
	}
	operator_int = masked_int & mask_2;
	if (operator_int > 0) {
		masked_int = operator_int;
		final_value = final_value << 16;
	}
	operator_int = masked_int & mask_3;
	if (operator_int > 0) {
		masked_int = operator_int;
		final_value = final_value << 8;
	}
	operator_int = masked_int & mask_4;
	if (operator_int > 0) {
		masked_int = operator_int;
		final_value = final_value << 4;
	}
	operator_int = masked_int & mask_5;
	if (operator_int > 0) {
		masked_int = operator_int;
		final_value = final_value << 2;
	}
	operator_int = masked_int & mask_6;
	if (operator_int > 0) {
		masked_int = operator_int;
		final_value = final_value << 1;
	}
	return final_value;
}



bool __VECTOR_MEMBER_FUNCTION_resize_reserve(struct Vector_generic* vector, uint64_t new_reserved_size_items) {
	if (vector == NULL) return throw_exception(INVALID_ARGUMENT,String("pointer passed to resize was null"));
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
	if (vector == NULL)			return throw_exception(INVALID_ARGUMENT,String("vector pointer was null"));
	if (vector->data == NULL)	return throw_exception(INVALID_ARGUMENT,String("vector has been deleted"));
	if (obj == NULL)			return throw_exception(INVALID_ARGUMENT,String("passed ptr to object is null"));
	///SANITY CHECK///

	uint64_t post_used_bytes = vector->size_bytes + vector->object_byte_size;
	if (post_used_bytes > vector->reserved_bytes) {
		bool resize_success = __VECTOR_MEMBER_FUNCTION_resize_reserve(vector, -1);
		if (!resize_success) return NULL;
	}
	void* item_position = vector->end;
	memcpy(item_position, obj, vector->object_byte_size);
	vector->size_bytes = post_used_bytes;
	vector->size += 1;
	vector->end = ((unsigned char*) vector->end) + vector->object_byte_size;
	return item_position;
}

bool __VECTOR_MEMBER_FUNCTION_pop_back(struct Vector_generic* vector) {
	if (vector == NULL)				return throw_exception(INVALID_ARGUMENT, String("vector pointer was null"));
	if (vector->data == NULL)		return throw_exception(INVALID_ARGUMENT, String("vector has been deleted"));
	if (vector->size == 0)			return throw_exception(UNDERFLOW, String("vector is empty"));
	///SANITY CHECK///

	vector->size -= 1;
	vector->size_bytes -= vector->object_byte_size;
	vector->end = (unsigned char*)vector->end - vector->object_byte_size;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_at(struct Vector_generic* vector, uint64_t index) {
	if (vector == NULL)				return throw_exception(INVALID_ARGUMENT, String("vector pointer was null"));
	if (vector->data == NULL)		return throw_exception(INVALID_ARGUMENT, String("vector has been deleted"));
	if (index<0 || index >= vector->size)		return throw_exception(OUT_OF_RANGE, String("vector indexing error, out of range"));
	///SANITY CHECK///

	return ((unsigned char*) vector->data) + index * vector->object_byte_size;
}

void* __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(struct Vector_generic* vector, uint64_t index) {
	return ((unsigned char*)vector->data) + index * vector->object_byte_size;
}

bool __VECTOR_MEMBER_FUNCTION_delete(struct Vector_generic* vector) {
	if (vector == NULL)				return throw_exception(INVALID_ARGUMENT, String("vector pointer was null"));
	if (vector->data == NULL)		return throw_exception(INVALID_ARGUMENT, String("vector has been deleted"));
	///SANITY CHECK///


	free(vector->data);
	vector->data = NULL;
	vector->reserved_bytes = 0;
	vector->reserved_size = 0;
	vector->size_bytes = 0;
	vector->size = 0;
}

bool __VECTOR_MEMBER_FUNCTION_remove(struct Vector_generic* vector, uint64_t starting_index, uint64_t length) {
	if (vector == NULL)							return throw_exception(INVALID_ARGUMENT, String("vector pointer was null"));
	if (vector->data == NULL)					return throw_exception(INVALID_ARGUMENT, String("vector has been deleted"));
	if (length < 0 || starting_index < 0)		return throw_exception(OUT_OF_RANGE, String("vector indexing error, out of range"));
	if (starting_index + length > vector->size)	return throw_exception(OUT_OF_RANGE, String("cannot remove more elements than exist from starting index to end"));
	///SANITY CHECK///

	uint64_t ending_index = starting_index + length;
	void* start = vector->at_ref(vector, starting_index);
	void* end = vector->at_ref(vector, ending_index); //this places the pointer on the element AFTER the final element removed

	uint64_t trailing_length = vector->size - ending_index;
	uint64_t removed_bytes = length * vector->object_byte_size;

	memcpy(start, end, trailing_length*vector->object_byte_size);
	vector->size -= length;
	vector->size_bytes -= removed_bytes;
	vector->end = (unsigned char*)vector->end - removed_bytes;

	return true;
}

bool __VECTOR_MEMBER_FUNCTION_splice(struct Vector_generic* vector, struct Vector_generic* other, uint64_t destination_index, uint64_t source_index, uint64_t length) {
	if (vector == NULL)														return throw_exception(INVALID_ARGUMENT, String("vector pointer was null"));
	if (vector->data == NULL)												return throw_exception(INVALID_ARGUMENT, String("vector has been deleted"));
	if (vector->object_byte_size != other->object_byte_size)				return throw_exception(INVALID_ARGUMENT, String("cannot splice items of different width together"));
	if (destination_index < 0 || destination_index >= vector->size)			return throw_exception(OUT_OF_RANGE, String("vector indexing error, out of range"));
	if (source_index < 0 || length < 0)										return throw_exception(OUT_OF_RANGE, String("vector indexing error, out of range"));
	if (source_index + length > other->size)								return throw_exception(OUT_OF_RANGE, String("vector indexing error, out of range"));

	///SANITY CHECK///

	uint64_t destination_final_length = vector->size + length;
	while (destination_final_length > vector->reserved_size) {
		vector->reserve(vector, -1);
	}
	void* destination_splice_start	= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index);
	void* destination_splice_end	= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index + length);
	void* source_splice_start		= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(other, source_index);

	uint64_t length_bytes = length * vector->object_byte_size;
	memcpy(destination_splice_end, destination_splice_start, length);
	memcpy(destination_splice_start, source_splice_start, length);

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
	memcpy(destination_splice_end, destination_splice_start, trailing_bytes);
	memcpy(destination_splice_start, source_splice_start, length);

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
	struct generic_vector* generic_vector = new_Vector_generic(sizeof(t));\
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
	if (c_str == NULL) return throw_exception(INVALID_ARGUMENT, String("cannot copy string from nullptr"));
	int length = strlen(c_str);
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, 0, 0, length);
	bool push = string->vector.push_back_ref(&string->vector,"\0"); 
	return splice_return && push;
}

bool __STRING_MEMBER_FUNCTION_assign_dynamic(struct String* string, char* c_str, uint64_t length) {
	if (c_str == NULL) return false;
	if (length < 0) return false;
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, 0, 0, length);
	bool push = string->vector.push_back_ref(&string->vector, "\0");
	return splice_return && push;
}

bool __STRING_MEMBER_FUNCTION_append(struct String* string, char* c_str) {
	string->vector.pop_back(&string->vector);
	int length = strlen(c_str);
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, string->vector.size, 0, length);
	bool push = string->vector.push_back_ref(&string->vector, "\0");
}

bool __STRING_MEMBER_FUNCTION_delete(struct String* self) {
	self->vector.delete(&self->vector);
}

struct String* new_String(char str[]) {
	struct String* string = malloc(sizeof(struct String));
	string->vector = Vector(char)(sizeof(char));
	string->size = &__STRING_MEMBER_FUNCTION_size;
	string->c_str = &__STRING_MEMBER_FUNCTION_c_str;
	string->assign = &__STRING_MEMBER_FUNCTION_assign;
	string->assign_dynamic = &__STRING_MEMBER_FUNCTION_assign_dynamic;
	string->delete = &__STRING_MEMBER_FUNCTION_delete;
	string->assign(string, str);
	string->append = &__STRING_MEMBER_FUNCTION_append;
	return string;
}


struct String String(char* str) {
	struct String* string = new_String(str);
	struct String return_object = *string;
	free(string);
	return return_object;
}

struct FixedTreeNode {
	struct FixedTreeNode* left;
	struct FixedTreeNode* right;
};

bool __FIXED_TREE_NODE_MEMBER_FUNCTION_delete(struct FixedTreeNode* node) {
	if (node != NULL) {
		__FIXED_TREE_NODE_MEMBER_FUNCTION_delete(node->left);
		__FIXED_TREE_NODE_MEMBER_FUNCTION_delete(node->right);
		free(node);
	} 
	return false;
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

void __FIXED_TREE_MEMBER_FUNCTION_remove_object(struct FixedTree* tree, uint64_t id) {
	struct FixedTreeNode* chain[63];
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
		*current_pointer = NULL;
	}
	index -= 1;
	bool do_deletion = false;
	while (index >= 0) {
		if (chain[index]->left != NULL && chain[index]->right != NULL) {
			do_deletion = true;
			break;
		}
		index--;
	}
	if (do_deletion == true) {
		__FIXED_TREE_NODE_MEMBER_FUNCTION_delete(chain[index]);
	}
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
	tree->head = new_FixedTreeNode();
	tree->find = &__FIXED_TREE_MEMBER_FUNCTION_find;
	tree->insert = &__FIXED_TREE_MEMBER_FUNCTION_insert;
	tree->remove = &__FIXED_TREE_MEMBER_FUNCTION_remove_object;
	tree->get_items = &__FIXED_TREE_MEMBER_FUNCTION_get_items;
	return tree;
}

void* throw_exception(uint64_t error_literal, struct String message) {
	struct String* error_message_base = error_table->find(error_table, error_literal);
	if (error_message_base == NULL) throw_exception(UNKNOWN_ERROR, message);
	struct String final_message = String("\n\nException!: ");
	for (int i = 0; i < final_message.vector.size; i++) {
		char chara = final_message.vector.at(&final_message.vector, i);
		printf("%i%c", i, chara);
	}
	final_message.append(&final_message, error_message_base->c_str(error_message_base));
	final_message.append(&final_message, ". ");
	final_message.append(&final_message, message.c_str(&message));
	final_message.append(&final_message, "\n\n");
	printf(final_message.c_str(&final_message));
	abort();
	return false;
}

void construct_error_table() {
	error_table = new_FixedTree(sizeof(struct String), NULL, NULL);
	error_table->insert(error_table, UNKNOWN_ERROR, new_String("Unknown error"));
	error_table->insert(error_table, OUT_OF_RANGE, new_String("Out of range"));
	error_table->insert(error_table, UNDERFLOW, new_String("Underflow error"));
	error_table->insert(error_table, OVERFLOW, new_String("Overflow error"));
	error_table->insert(error_table, INVALID_ARGUMENT, new_String("Invalid argument"));
}

int main()
{	
	construct_error_table();


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
