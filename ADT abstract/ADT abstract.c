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

typedef unsigned long uint64_t;
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

struct Vector_generic {
	int object_byte_size;
	long size;
	long reserved_size;
	long size_bytes;
	long reserved_bytes;
	void* data;
	void* end;
	bool (*reserve)(struct Vector_generic* self, long size) ;
	void* (*push_back_ref)(struct Vector_generic* self, void* item);
	bool (*pop_back)(struct Vector_generic* self);
	void* (*at_ref)(struct Vector_generic* self, long index);
	bool (*remove)(struct Vector_generic* self, long starting_index, long length);
	bool (*delete)(struct Vector_generic* self);
};



//searches for the most significant bit using bitmasks. I just think this algorithm is fun.
unsigned long get_MSB(unsigned long value) {
	unsigned long mask_1 = 0xffffffff00000000; 
	unsigned long mask_2 = 0xffff0000ffff0000;
	unsigned long mask_3 = 0xff00ff00ff00ff00;
	unsigned long mask_4 = 0xf0f0f0f0f0f0f0f0;
	unsigned long mask_5 = 0xcccccccccccccccc;
	unsigned long mask_6 = 0xaaaaaaaaaaaaaaaa;

	unsigned long masked_int = value;
	unsigned long operator_int = masked_int & mask_1;
	unsigned long final_value = 1;
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



bool __VECTOR_MEMBER_FUNCTION_resize_reserve(struct Vector_generic* vector, long new_reserved_size_items) {
	if (vector == NULL) return false;
	///SANITY CHECK///

	long new_reserved_size_bytes = 0;
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
	if (vector == NULL)			return NULL;
	if (vector->data == NULL)	return NULL;
	if (obj == NULL)			return NULL;
	///SANITY CHECK///

	long post_used_bytes = vector->size_bytes + vector->object_byte_size;
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
	if (vector == NULL)				return false;
	if (vector->data == NULL)		return false;
	if (vector->size == 0)	return false;
	///SANITY CHECK///

	vector->size -= 1;
	vector->size_bytes -= vector->object_byte_size;
	vector->end = (unsigned char*)vector->end - vector->object_byte_size;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_at(struct Vector_generic* vector, long index) {
	if (vector == NULL)							return NULL;
	if (vector->data == NULL)					return NULL;
	if (index<0 || index >= vector->size) return NULL;
	///SANITY CHECK///

	return ((unsigned char*) vector->data) + index * vector->object_byte_size;
}

void* __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(struct Vector_generic* vector, long index) {
	return ((unsigned char*)vector->data) + index * vector->object_byte_size;
}

bool __VECTOR_MEMBER_FUNCTION_delete(struct Vector_generic* vector) {
	if (vector == NULL)			return false;
	if (vector->data == NULL)	return false;
	///SANITY CHECK///


	free(vector->data);
	vector->data = NULL;
	vector->reserved_bytes = 0;
	vector->reserved_size = 0;
	vector->size_bytes = 0;
	vector->size = 0;
}

bool __VECTOR_MEMBER_FUNCTION_remove(struct Vector_generic* vector, long starting_index, long length) {
	if (vector == NULL)									return false;
	if (vector->data == NULL)							return false;
	if (length < 0 || starting_index < 0)				return false;
	if (starting_index + length > vector->size)	return false;
	///SANITY CHECK///

	long ending_index = starting_index + length;
	void* start = vector->at_ref(vector, starting_index);
	void* end = vector->at_ref(vector, ending_index); //this places the pointer on the element AFTER the final element removed

	long trailing_length = vector->size - ending_index;
	long removed_bytes = length * vector->object_byte_size;

	memcpy(start, end, trailing_length*vector->object_byte_size);
	vector->size -= length;
	vector->size_bytes -= removed_bytes;
	vector->end = (unsigned char*)vector->end - removed_bytes;

	return true;
}

bool __VECTOR_MEMBER_FUNCTION_splice(struct Vector_generic* vector, struct Vector_generic* other, long destination_index, long source_index, long length) {
	if (vector == NULL || other == NULL)									return false;
	if (vector->data == NULL || other->data == NULL)						return false;
	if (vector->object_byte_size != other->object_byte_size)				return false;
	if (destination_index < 0 || destination_index >= vector->size)	return false;
	if (source_index < 0 || length < 0)										return false;
	if (source_index + length > other->size)							return false;

	///SANITY CHECK///

	long destination_final_length = vector->size + length;
	while (destination_final_length > vector->reserved_size) {
		vector->reserve(vector, -1);
	}
	void* destination_splice_start	= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index);
	void* destination_splice_end	= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index + length);
	void* source_splice_start		= __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(other, source_index);

	long length_bytes = length * vector->object_byte_size;
	memcpy(destination_splice_end, destination_splice_start, length);
	memcpy(destination_splice_start, source_splice_start, length);

	vector->size += length;
	vector->size_bytes += length_bytes;

	return true;
}

bool __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(struct Vector_generic* vector, void* other, long destination_index, long source_index, long length) {
	///SANITY CHECK///

	long destination_final_length = vector->size + length;
	while (destination_final_length > vector->reserved_size) {
		vector->reserve(vector, -1);
	}
	void* destination_splice_start =	__VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index);
	void* destination_splice_end =		__VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index + length);
	void* source_splice_start =			(unsigned char*) other + source_index*vector->object_byte_size;

	long length_bytes = length * vector->object_byte_size;
	long trailing_bytes = (unsigned char*) vector->end - destination_splice_start;
	memcpy(destination_splice_end, destination_splice_start, trailing_bytes);
	memcpy(destination_splice_start, source_splice_start, length);

	vector->size += length;
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

#define GENERATE_TYPED_VECTOR_DECLARATION(t) \
struct Vector(t) {\
int object_byte_size;\
long size;\
long reserved_size;\
long size_bytes;\
long reserved_bytes;\
t* data;\
t* end;\
bool (*reserve)(struct Vector_generic* self, long size);\
t* (*push_back_ref)(struct Vector(t)* self, t* item);\
bool (*pop_back)(struct Vector_generic* self);\
t* (*at_ref)(struct Vector(t)* self, long index);\
bool (*remove)(struct Vector_generic* self, long starting_index, long length);\
bool (*delete)(struct Vector_generic* self);\
t (*at)(struct Vector(t)* self, long index);\
t* (*push_back)(struct Vector(t)* self, t item);\
};\
\
t* GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_ref_,t)(struct Vector(t)* vector,long index){\
	return __VECTOR_MEMBER_FUNCTION_at(vector,index);\
}\
t GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_, t)(struct Vector(t)* vector, long index) {\
		return *GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_ref_,t)(vector,index);\
}\
t* GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_ref_,t)(struct Vector(t)* vector, t* item){\
	return __VECTOR_MEMBER_FUNCTION_push_back(vector,item);\
}\
t* GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_,t)(struct Vector(t)* vector, t item){\
	return GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_ref_,t)(vector,&item);\
}\
\
struct Vector(t)* new_Vector(t)() {\
	struct generic_vector* generic_vector = new_Vector_generic(sizeof(t));\
	struct Vector(t)* vector = malloc(sizeof(struct Vector(t)));\
	memcpy(vector,generic_vector,sizeof(struct Vector_generic));\
	free(generic_vector);\
	vector->at_ref = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_ref_,t);\
	vector->at = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_,t);\
	vector->push_back_ref = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_ref_,t);\
	vector->push_back = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_,t);\
	return vector;\
}\
struct Vector(t) Vector(t)(){\
	struct Vector(t)* vec_ptr = new_Vector(t)();\
	struct Vector(t) solid_vec = *vec_ptr;\
	free(vec_ptr);\
	return solid_vec;\
}

GENERATE_TYPED_VECTOR_DECLARATION(int)
GENERATE_TYPED_VECTOR_DECLARATION(char)
/*
struct Vector(char) {
	\
		int object_byte_size;\
		long size;\
		long reserved_size;\
		long size_bytes;\
		long reserved_bytes;\
		char* data;\
		char* end;\
		bool (*reserve)(struct Vector_generic* self, long size);\
		char* (*push_back_ref)(struct Vector(char) * self, char * item);\
		bool (*pop_back)(struct Vector_generic* self);\
		char* (*at_ref)(struct Vector(char) * self, long index);\
		bool (*remove)(struct Vector_generic* self, long starting_index, long length);\
		bool (*delete)(struct Vector_generic* self);\
		char(*at)(struct Vector(char) * self, long index);\
		char* (*push_back)(struct Vector(char) * self, char item);\
};\
\
char* GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_ref_, char)(struct Vector(char)* vector, long index) {
		\
			return __VECTOR_MEMBER_FUNCTION_at(vector, index);\
	}\
		char GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_, char)(struct Vector(char)* vector, long index) {
			\
				return *GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_ref_, char)(vector, index);\
		}\
			char* GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_ref_, char)(struct Vector(char)* vector, char* item) {
				\
					return __VECTOR_MEMBER_FUNCTION_push_back(vector, item);\
			}\
				char* GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_, char)(struct Vector(char)* vector, char item) {
					\
						return GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_ref_, char)(vector, &item);\
				}\
					\
						struct Vector(char)* new_Vector(char)() {
						\
							struct generic_vector* generic_vector = new_Vector_generic(sizeof(char));\
							struct ector(t) = malloc(sizeof)
							generic_vector->at_ref = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_ref_, char);\
							generic_vector->at = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_at_, char);\
							generic_vector->push_back_ref = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_ref_, char);\
							generic_vector->push_back = &GLUE(__AUTO_VECTOR_MEMBER_FUNCTION_push_back_, char);\
							return generic_vector;\
					}\
						struct Vector(char) Vector(char)() {
							\
								struct Vector(char)* vec_ptr = new_Vector(char)();\
								struct Vector(char) solid_vec = *vec_ptr;\
								free(vec_ptr);\
								return solid_vec;\
						}
*/
struct String {
	struct Vector(char) vector;
	long  (*size)(struct String* self);
	char* (*c_str)(struct String* self);
	bool  (*assign)(struct String* self, char c_str[]);
	bool  (*assign_dynamic)(struct String* self, char* c_str, long length);
	bool  (*delete)(struct String* self);
};

long __STRING_MEMBER_FUNCTION_size(struct String* string){
	return string->vector.size-1;
}

char* __STRING_MEMBER_FUNCTION_c_str(struct String* string){
	return string->vector.data;
}

bool __STRING_MEMBER_FUNCTION_assign(struct String* string, char c_str[]) {
	if (c_str == NULL) return false;
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, 0, 0, sizeof(c_str));
	bool push = string->vector.push_back(&string->vector,"\0");
	return splice_return && push;
}

bool __STRING_MEMBER_FUNCTION_assign_dynamic(struct String* string, char* c_str, long length) {
	if (c_str == NULL) return false;
	if (length < 0) return false;
	bool splice_return = __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->vector, c_str, 0, 0, length);
	bool push = string->vector.push_back(&string->vector, "\0");
	return splice_return && push;
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
	return string;
}


struct String String(char str[]) {
	struct String* string = new_String(str);
	struct String return_object = *string;
	free(string);
	return return_object;
}

struct FixedTreeNode {
	void* left;
	void* right;
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
	unsigned long object_byte_width;
	bool (*__designated_copy_function)(void* object_ptr, void* destination);
	bool (*__designated_delete_function)(void* object_ptr);
};

//note the final return case of return node actually returns a pointer to the object at the end, not a node.
void** __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(struct FixedTreeNode** node, unsigned long navkey, unsigned long* bitmask) {
	if (*node == NULL) return node;
	if (bitmask == 0) {
		return node;
	}
	bool go_left = (navkey & *bitmask) == 0;
	*bitmask = *bitmask << 1;
	if (go_left) {
		return __FIXED_TREE_MEMBER_FUNCTION_navigate(&((*node)->left), navkey, bitmask);
	}
	else {
		return __FIXED_TREE_MEMBER_FUNCTION_navigation(&((*node)->right), navkey, bitmask);
	}
}

bool __FIXED_TREE_MEMBER_FUNCTION_insert(struct FixedTree* tree, unsigned long navkey, void* insert_object) {
	unsigned long bitmask = 1;
	void** end_node_ptr = __FIXED_TREE_MEMBER_FUNCTION_navigate_internal(&tree->head, navkey, &bitmask);
	//end_node_ptr points at the ptr to the end of the tree. If an item already exists there, it will not be NULL.
	//in that case, bitmask is 0.
	//otherwise, we need to create a new node, and set end_node_ptr to point at that node. We then want to navigate down that node and repeat.
	while (bitmask != 0) {
		*end_node_ptr = new_FixedTreeNode();
		__FIXED_TREE_MEMBER_FUNCTION_navigate_internal(end_node_ptr, navkey, &bitmask);
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

void __FIXEDTREE_MEMBER_FUNCTION_remove_object(struct FixedTree* tree, unsigned long id) {
	struct FixedTreeNode* chain[63];
	int index = 0;
	unsigned long bitmask = 1;
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

struct Vector_generic __FIXED_TREE_MEMBER_FUNCTION_get_items(struct FixedTree* tree) {
	struct Vector_generic output = Vector_generic(sizeof(Pair_generic));
	struct FixedTreeNode* chain[65];
	char state_chain[65];
	chain[0] = tree->head;
	state_chain[0] = 0;
	int i = 0;
	while (i > -1) {
		if (i == 65) {
			output.push_back_ref(&output, Pair(uint64_t, void_ptr)(chain[i]));
		}
		if (state_chain[i] == 0) {
			state_chain[i] = 1;
			if (chain[i]->left != NULL) {
				state_chain[i + 1] = 0;
				chain[i + 1] = chain[i]->left;
				i++;
			}
			continue;
		}
		if (state_chain[i] == 1) {
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
}





struct FixedTree* new_FixedTree(long object_byte_width, bool (*copy_function)(void* object_ptr, void* destination), bool(*delete_function)(void* object_ptr)) {
	struct FixedTree* tree = malloc(sizeof(struct FixedTree));
	tree->__designated_copy_function = copy_function;
	tree->head = new_FixedTreeNode();

}



//struct Vector errors;
//void build_error_table() {
//	errors = Vector(sizeof(struct String));
//	errors.push_back(&errors,"Out of Bounds exception");
//}
//
//void error_handler(int signal) {
//
//}

int main()
{	
	
	struct Vector(int)* a = new_Vector_int();
	for (int i = 0; i < 1000; i++) {
		a->push_back(a, &i);
	}
	for (int i = 0; i < 1000; i++) {
		int was = a->at(a, i);
		*a->at_ref(a, i) = (1000 - i);
		int now = a->at(a, i);
		printf("value was %i, now %i\n",was,now);
	}

	struct String test = String("hi world");
	printf(test.c_str(&test));
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
