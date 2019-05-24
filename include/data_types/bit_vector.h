#ifndef BIT_VECTOR             
#define BIT_VECTOR

#include "data_types/primitive_types.h"
#undef KMALLOC_H
#include "memory_manager/kmalloc.h"

typedef char t_bit_vector;

inline static __attribute__((always_inline)) t_bit_vector* bit_vector_init(u32 size)
{
	int i = 0;
	t_bit_vector* bit_vector;

	bit_vector = kmalloc(size);
	for (i=0 ; i < size; i++)
	{
		bit_vector[i] = 0;
	}
	return bit_vector;
}

inline static __attribute__((always_inline)) void bit_vector_re_init(t_bit_vector* bit_vector,u32 size)
{
	int i;

	for (i=0 ; i < size; i++)
	{
		bit_vector[i] = 0;
	}
}

inline static __attribute__((always_inline)) void bit_vector_free(t_bit_vector* bit_vector)
{	
	kfree(bit_vector);
}

inline static __attribute__((always_inline)) void bit_vector_set(t_bit_vector* bit_vector,u32 index)
{
	bit_vector[index] = 1;
}

inline static __attribute__((always_inline)) void bit_vector_reset(t_bit_vector* bit_vector,u32 index)
{
	bit_vector[index] = 0;	
}

inline static __attribute__((always_inline)) u8 bit_vector_get(t_bit_vector* bit_vector,u32 index)
{
	return bit_vector[index];
}

#endif
