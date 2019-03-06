#ifndef BIT_VECTOR             
#define BIT_VECTOR

#include "data_types/primitive_types.h"
#undef KMALLOC_H
#include "memory_manager/kmalloc.h"

typedef char t_bit_vector;

//inline extern __attribute__((always_inline)) t_bit_vector* bit_vector_init(u32 size)
inline static __attribute__((always_inline)) t_bit_vector* bit_vector_init(u32 size)
{
	int i = 0;
	t_bit_vector* bit_vector;

	bit_vector = kmalloc((size/8)+1);
	for (i=0 ; i < ((size/8)+1); i++)
	{
		bit_vector[i] = 0;
	}
	return bit_vector;
}

//inline extern __attribute__((always_inline)) void bit_vector_re_init(t_bit_vector* bit_vector,u32 size)
inline static __attribute__((always_inline)) void bit_vector_re_init(t_bit_vector* bit_vector,u32 size)
{
	int i;

	for (i=0 ; i < ((size/8)+1); i++)
	{
		bit_vector[i] = 0;
	}
}

//inline extern __attribute__((always_inline)) void bit_vector_free(t_bit_vector* bit_vector)
inline static __attribute__((always_inline)) void bit_vector_free(t_bit_vector* bit_vector)
{	
	kfree(bit_vector);
}

//inline extern __attribute__((always_inline)) void bit_vector_set(t_bit_vector* bit_vector,u32 index)
inline static __attribute__((always_inline)) void bit_vector_set(t_bit_vector* bit_vector,u32 index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	bit_vector[block_index] |= (1 << block_offset);
}

//inline extern __attribute__((always_inline)) void bit_vector_reset(t_bit_vector* bit_vector,u32 index)
inline static __attribute__((always_inline)) void bit_vector_reset(t_bit_vector* bit_vector,u32 index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	bit_vector[block_index] &= (~(1 << block_offset));	
}

//inline extern __attribute__((always_inline)) u8 bit_vector_get(t_bit_vector* bit_vector,u32 index)
inline static __attribute__((always_inline)) u8 bit_vector_get(t_bit_vector* bit_vector,u32 index)
{
	u8 ret;
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	ret =  bit_vector[block_index] >> block_offset;
	return ret & 1;
}

//inline extern __attribute__((always_inline)) u8 bit_vector_reset_if_set(t_bit_vector* bit_vector,u32 index)
inline static __attribute__((always_inline)) u8 bit_vector_reset_if_set(t_bit_vector* bit_vector,u32 index)
{
	u8 ret;
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	ret =  bit_vector[block_index] >> block_offset;
	if (ret == 1)
	{
		bit_vector[block_index] &= (~(1 << block_offset));
	}
	return ret & 1;
}
#endif
