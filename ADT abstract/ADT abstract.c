// ADT abstract.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdbool.h>


struct Vector {
	int object_byte_size;
	long used_items;
	long reserved_items;
	long used_bytes;
	long reserved_bytes;
	void* data;
	void* end;
	bool (*reserve)(struct Vector*, long) ;
	void* (*push_back)(struct Vector*, void*);
	bool (*pop_back)(struct Vector*);
	void* (*at)(struct Vector*, long);
	bool (*delete)(struct Vector*);
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

bool __VECTOR_MEMBER_FUNCTION_resize_reserve(struct Vector* vector, long new_reserved_size_items) {
	if (vector == NULL) return false;
	if (vector->data == NULL) return NULL;
	if (new_reserved_size_items < vector->used_items) {
		return false;
	}
	long new_reserved_size_bytes = 0;
	if (new_reserved_size_items == -1) {
		new_reserved_size_items = vector->reserved_items << 1; 
		new_reserved_size_bytes = vector->reserved_bytes << 1;
	}
	else {
		new_reserved_size_bytes = new_reserved_size_items * vector->object_byte_size;
	}

	void* current_memory = vector->data;
	void* new_memory = malloc(new_reserved_size_bytes);
	if (new_memory == NULL) return false;
	if (current_memory != NULL) {
		memcpy(new_memory, current_memory, vector->used_bytes);
		free(current_memory);
	}
	vector->reserved_bytes = new_reserved_size_bytes;
	vector->reserved_items = new_reserved_size_items;
	vector->data = new_memory;
	vector->end = (unsigned char*) vector->data + vector->used_bytes;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_push_back(struct Vector* vector, void* obj) {
	if (vector == NULL) return NULL;
	if (vector->data == NULL) return NULL;
	if (obj == NULL) return NULL;
	long post_used_bytes = vector->used_bytes + vector->object_byte_size;
	if (post_used_bytes > vector->reserved_bytes) {
		bool resize_success = __VECTOR_MEMBER_FUNCTION_resize_reserve(vector, -1);
		if (!resize_success) return NULL;
	}
	void* item_position = vector->end;
	memcpy(item_position, obj, vector->object_byte_size);
	vector->used_bytes = post_used_bytes;
	vector->used_items += 1;
	vector->end = ((unsigned char) vector->end) + vector->object_byte_size;
	return item_position;
}

bool __VECTOR_MEMBER_FUNCTION_pop_back(struct Vector* vector) {
	if (vector == NULL) return false;
	if (vector->data == NULL) return false;
	if (vector->used_items == 0) return false;
	vector->used_items -= 1;
	vector->used_bytes -= vector->object_byte_size;
	vector->end = (unsigned char)vector->end - vector->object_byte_size;
	return true;
}

void* __VECTOR_MEMBER_FUNCTION_at(struct Vector* vector, long index) {
	if (vector == NULL) return NULL;
	if (vector->data == NULL) return NULL;
	if (index<0 || index >= vector->used_items) return NULL;
	return ((unsigned char*) vector->data) + index * vector->object_byte_size;
}

bool __VECTOR_MEMBER_FUNCTION_delete(struct Vector* vector) {
	if (vector == NULL) return false;
	if (vector->data == NULL) return false;
	free(vector->data);
	vector->reserved_bytes = 0;
	vector->reserved_items = 0;
	vector->used_bytes = 0;
	vector->used_items = 0;
}

static struct Vector* Vector(int object_byte_size) {
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->object_byte_size = object_byte_size;
	vector->reserved_bytes = 0;
	vector->reserved_items = 0;
	vector->used_bytes = 0;
	vector->used_items;
	vector->reserve = &__VECTOR_MEMBER_FUNCTION_resize_reserve;
	vector->push_back = &__VECTOR_MEMBER_FUNCTION_push_back;
	vector->pop_back = &__VECTOR_MEMBER_FUNCTION_pop_back;
	vector->at = &__VECTOR_MEMBER_FUNCTION_at;
	vector->delete = &__VECTOR_MEMBER_FUNCTION_delete;
}

int main()
{
	struct Vector* a = Vector(sizeof(int));
	for (int i = 0; i < 1000; i++) {
		a->push_back(a, &i);
	}
	for (int i = 0; i < 1000; i++) {
		int was = *(int*)a->at(a, i);
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
