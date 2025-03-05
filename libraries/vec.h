#ifndef vec_h
#define vec_h

#ifdef __cpp_decltype
#include <type_traits>
#define typeof(T) std::remove_reference<std::add_lvalue_reference<decltype(T)>::type>::type
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

// generic type for internal use
typedef void* vector;
// number of elements in a vector
typedef size_t vec_size_t;
// number of bytes for a type
typedef size_t vec_type_t;

// TODO: more rigorous check for typeof support with different compilers
#if _MSC_VER == 0 || __STDC_VERSION__ >= 202311L || defined __cpp_decltype

// shortcut defines

// vec_addr is a vector* (aka type**)
#define vector_add_dst(vec_addr)\
	((typeof(*vec_addr))(\
	    _vector_add_dst((vector*)vec_addr, sizeof(**vec_addr))\
	))
#define vector_insert_dst(vec_addr, pos)\
	((typeof(*vec_addr))(\
	    _vector_insert_dst((vector*)vec_addr, sizeof(**vec_addr), pos)))

#define vector_add(vec_addr, value)\
	(*vector_add_dst(vec_addr) = value)
#define vector_insert(vec_addr, pos, value)\
	(*vector_insert_dst(vec_addr, pos) = value)

#else

#define vector_add_dst(vec_addr, type)\
	((type*)_vector_add_dst((vector*)vec_addr, sizeof(type)))
#define vector_insert_dst(vec_addr, type, pos)\
	((type*)_vector_insert_dst((vector*)vec_addr, sizeof(type), pos))

#define vector_add(vec_addr, type, value)\
	(*vector_add_dst(vec_addr, type) = value)
#define vector_insert(vec_addr, type, pos, value)\
	(*vector_insert_dst(vec_addr, type, pos) = value)

#endif

// vec is a vector (aka type*)
#define vector_erase(vec, pos, len)\
	(_vector_erase((vector)vec, sizeof(*vec), pos, len))
#define vector_remove(vec, pos)\
	(_vector_remove((vector)vec, sizeof(*vec), pos))

#define vector_reserve(vec_addr, capacity)\
	(_vector_reserve((vector*)vec_addr, sizeof(**vec_addr), capacity))

#define vector_copy(vec)\
	(_vector_copy((vector)vec, sizeof(*vec)))

vector vector_create(void);

void vector_free(vector vec);

void* _vector_add_dst(vector* vec_addr, vec_type_t type_size);

void* _vector_insert_dst(vector* vec_addr, vec_type_t type_size, vec_size_t pos);

void _vector_erase(vector vec_addr, vec_type_t type_size, vec_size_t pos, vec_size_t len);

void _vector_remove(vector vec_addr, vec_type_t type_size, vec_size_t pos);

void vector_pop(vector vec);

void _vector_reserve(vector* vec_addr, vec_type_t type_size, vec_size_t capacity);

vector _vector_copy(vector vec, vec_type_t type_size);

vec_size_t vector_size(vector vec);

vec_size_t vector_capacity(vector vec);

// closing bracket for extern "C"
#ifdef __cplusplus
}
#endif

#endif /* vec_h */