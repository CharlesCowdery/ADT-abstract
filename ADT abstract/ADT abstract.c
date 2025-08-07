// ADT abstract.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

struct Vector {
	int object_byte_size;
	long size;
	long reserved_size;
	long size_bytes;
	long reserved_bytes;
	void* data;
	void* end;
	bool (*reserve)(struct Vector* self, long size) ;
	void* (*push_back)(struct Vector* self, void* item);
	bool (*pop_back)(struct Vector* self);
	void* (*at)(struct Vector* self, long index);
	bool (*remove)(struct Vector* self, long starting_index, long length);
	bool (*delete)(struct Vector* self);
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


void error_handler(int sig) {

}

bool __VECTOR_MEMBER_FUNCTION_resize_reserve(struct Vector* vector, long new_reserved_size_items) {
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
	if (new_reserved_size_items < vector->size) {
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

void* __VECTOR_MEMBER_FUNCTION_push_back(struct Vector* vector, void* obj) {
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

bool __VECTOR_MEMBER_FUNCTION_pop_back(struct Vector* vector) {
	if (vector == NULL)				return false;
	if (vector->data == NULL)		return false;
	if (vector->size == 0)	return false;
	///SANITY CHECK///

	vector->size -= 1;
	vector->size_bytes -= vector->object_byte_size;
	vector->end = (unsigned char*)vector->end - vector->object_byte_size;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_at(struct Vector* vector, long index) {
	if (vector == NULL)							return NULL;
	if (vector->data == NULL)					return NULL;
	if (index<0 || index >= vector->size) return NULL;
	///SANITY CHECK///

	return ((unsigned char*) vector->data) + index * vector->object_byte_size;
}

void* __VECTOR_MEMBER_FUNCTION_at_DANGEROUS(struct Vector* vector, long index) {
	return ((unsigned char*)vector->data) + index * vector->object_byte_size;
}

bool __VECTOR_MEMBER_FUNCTION_delete(struct Vector* vector) {
	if (vector == NULL)			return false;
	if (vector->data == NULL)	return false;
	///SANITY CHECK///


	free(vector->data);
	vector->reserved_bytes = 0;
	vector->reserved_size = 0;
	vector->size_bytes = 0;
	vector->size = 0;
}

bool __VECTOR_MEMBER_FUNCTION_remove(struct Vector* vector, long starting_index, long length) {
	if (vector == NULL)									return false;
	if (vector->data == NULL)							return false;
	if (length < 0 || starting_index < 0)				return false;
	if (starting_index + length > vector->size)	return false;
	///SANITY CHECK///

	long ending_index = starting_index + length;
	void* start = vector->at(vector, starting_index);
	void* end = vector->at(vector, ending_index); //this places the pointer on the element AFTER the final element removed

	long trailing_length = vector->size - ending_index;
	long removed_bytes = length * vector->object_byte_size;

	memcpy(start, end, trailing_length*vector->object_byte_size);
	vector->size -= length;
	vector->size_bytes -= removed_bytes;
	vector->end = (unsigned char*)vector->end - removed_bytes;

	return true;
}

bool __VECTOR_MEMBER_FUNCTION_splice(struct Vector* vector, struct Vector* other, long destination_index, long source_index, long length) {
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

bool __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(struct Vector* vector, void* other, long destination_index, long source_index, long length) {
	///SANITY CHECK///

	long destination_final_length = vector->size + length;
	while (destination_final_length > vector->reserved_size) {
		vector->reserve(vector, -1);
	}
	void* destination_splice_start =	__VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index);
	void* destination_splice_end =		__VECTOR_MEMBER_FUNCTION_at_DANGEROUS(vector, destination_index + length);
	void* source_splice_start =			(unsigned char*) other + source_index*vector->object_byte_size;

	long length_bytes = length * vector->object_byte_size;
	memcpy(destination_splice_end, destination_splice_start, length);
	memcpy(destination_splice_start, source_splice_start, length);

	vector->size += length;
	vector->size_bytes += length_bytes;

	return true;
}

struct Vector* Vector(int object_byte_size) {
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->data = NULL;
	vector->object_byte_size = object_byte_size;
	vector->reserved_bytes = 0;
	vector->reserved_size = 0;
	vector->size_bytes = 0;
	vector->size = 0;
	vector->reserve = &__VECTOR_MEMBER_FUNCTION_resize_reserve;
	vector->push_back = &__VECTOR_MEMBER_FUNCTION_push_back;
	vector->pop_back = &__VECTOR_MEMBER_FUNCTION_pop_back;
	vector->at = &__VECTOR_MEMBER_FUNCTION_at;
	vector->delete = &__VECTOR_MEMBER_FUNCTION_delete;
	vector->remove = &__VECTOR_MEMBER_FUNCTION_remove;
	vector->reserve(vector, 1);
	return vector;
}

struct String {
	struct Vector basis;
	long  (*size)(struct String* self);
	char* (*c_str)(struct String* self);
	bool  (*assign)(struct String* self, char c_str[]);
	bool  (*assign_dynamic)(struct String* self, char* c_str, long length);
};

long __STRING_MEMBER_FUNCTION_size(struct String* string){
	return string->basis.size;
}

char* __STRING_MEMBER_FUNCTION_c_str(struct String* string){
	return string->basis.data;
}

bool __STRING_MEMBER_FUNCTION_assign(struct String* string, char c_str[]) {
	if (c_str == NULL) return false;
	return __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->basis, c_str, 0, 0, sizeof(c_str));
}

bool __STRING_MEMBER_FUNCTION_assign_dynamic(struct String* string, char* c_str, long length) {
	if (c_str == NULL) return false;
	if (length < 0) return false;
	return __VECTOR_MEMBER_FUNCTION_splice_DANGEROUS(&string->basis, c_str, 0, 0, length);
}

struct String* String(char str[]) {
	struct String* string = malloc(sizeof(struct String));
	string->basis = *Vector(sizeof(char));
	string->size = &__STRING_MEMBER_FUNCTION_size;
	string->c_str = &__STRING_MEMBER_FUNCTION_c_str;
	string->assign = &__STRING_MEMBER_FUNCTION_assign;
	string->assign_dynamic = &__STRING_MEMBER_FUNCTION_assign_dynamic;
	string->assign(string,str);

}

int main()
{
	struct Vector* a = Vector(sizeof(int));
	for (int i = 0; i < 1000; i++) {
		a->push_back(a, &i);
	}
	for (int i = 0; i < 1000; i++) {
		void* position = a->at(a, i);
		int was = *(int*)position;
		*(int*) a->at(a, i) = (1000 - i);
		int now = *(int*)a->at(a, i);
		printf("value was %i, now %i\n",was,now);
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
