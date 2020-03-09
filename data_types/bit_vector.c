#include "data_types/bit_vector.h"
#include "memory_manager/kmalloc.h"

t_bit_vector* bit_vector_init(u32 size)
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

void bit_vector_re_init(t_bit_vector* bit_vector,u32 size)
{
	int i;

	for (i=0 ; i < ((size/8)+1); i++)
	{
		bit_vector[i] = 0;
	}
}

//int static inline  __attribute__((always_inline)) foo();

void bit_vector_free(t_bit_vector* bit_vector)
{	
	kfree(bit_vector);
	foo();
}

void bit_vector_set(t_bit_vector* bit_vector,u32 index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	bit_vector[block_index] |= (1 << block_offset);
}

void bit_vector_reset(t_bit_vector* bit_vector,u32 index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	bit_vector[block_index] &= (~(1 << block_offset));	
}

u8 bit_vector_get(t_bit_vector* bit_vector,u32 index)
{
	u8 ret;
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	ret =  bit_vector[block_index] >> block_offset;
	return ret & 1;
}
