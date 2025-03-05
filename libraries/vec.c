#include <string.h>
#include <stdbool.h>
#include "vec.h"

typedef struct
{
	vec_size_t size;
	vec_size_t capacity;
	unsigned char data[]; 
} vector_header;

vector_header* vector_get_header(vector vec) { return &((vector_header*)vec)[-1]; }

vector vector_create(void)
{
	vector_header* h = (vector_header*)malloc(sizeof(vector_header));
	h->capacity = 0;
	h->size = 0;

	return &h->data;
}

void vector_free(vector vec) { free(vector_get_header(vec)); }

vec_size_t vector_size(vector vec) { return vector_get_header(vec)->size; }

vec_size_t vector_capacity(vector vec) { return vector_get_header(vec)->capacity; }

vector_header* vector_realloc(vector_header* h, vec_type_t type_size)
{
	vec_size_t new_capacity = (h->capacity == 0) ? 1 : h->capacity * 2;
	vector_header* new_h = (vector_header*)realloc(h, sizeof(vector_header) + new_capacity * type_size);
	new_h->capacity = new_capacity;

	return new_h;
}

bool vector_has_space(vector_header* h)
{
	return h->capacity - h->size > 0;
}

void* _vector_add_dst(vector* vec_addr, vec_type_t type_size)
{
	vector_header* h = vector_get_header(*vec_addr);

	if (!vector_has_space(h))
	{
		h = vector_realloc(h, type_size);
		*vec_addr = h->data;
	}

	return &h->data[type_size * h->size++];
}

void* _vector_insert_dst(vector* vec_addr, vec_type_t type_size, vec_size_t pos)
{
	vector_header* h = vector_get_header(*vec_addr);

	vec_size_t new_length = h->size + 1;

	// make sure there is enough room for the new element
	if (!vector_has_space(h))
	{
		h = vector_realloc(h, type_size);
		*vec_addr = h->data;
	}
	// move trailing elements
	memmove(&h->data[(pos + 1) * type_size],
		&h->data[pos * type_size],
		(h->size - pos) * type_size);

	h->size = new_length;

	return &h->data[pos * type_size];
}

void _vector_erase(vector vec, vec_type_t type_size, vec_size_t pos, vec_size_t len)
{
	vector_header* h = vector_get_header(vec);
	memmove(&h->data[pos * type_size],
		&h->data[(pos + len) * type_size],
		(h->size - pos - len) * type_size);

	h->size -= len;
}

void _vector_remove(vector vec, vec_type_t type_size, vec_size_t pos)
{
	_vector_erase(vec, type_size, pos, 1);
}

void vector_pop(vector vec) { --vector_get_header(vec)->size; }

void _vector_reserve(vector* vec_addr, vec_type_t type_size, vec_size_t capacity)
{
	vector_header* h = vector_get_header(*vec_addr);
	if (h->capacity >= capacity)
	{
		return;
	}

	h = (vector_header*)realloc(h, sizeof(vector_header) + capacity * type_size);
	h->capacity = capacity;
	*vec_addr = &h->data;
}

vector _vector_copy(vector vec, vec_type_t type_size)
{
	vector_header* h = vector_get_header(vec);
	size_t alloc_size = sizeof(vector_header) + h->size * type_size;
	vector_header* copy_h = (vector_header*)malloc(alloc_size);
	memcpy(copy_h, h, alloc_size);
	copy_h->capacity = copy_h->size;

	return &copy_h->data;
}