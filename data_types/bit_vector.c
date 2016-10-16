#include "memory_manager/kmalloc.h"

typedef char t_bit_vector;

t_bit_vector* bit_vector_init(u32 size)
{
	t_bit_vector* bit_vector;

	bit_vector = kmalloc(size);
	return bit_vector;
}

void bit_vector_free(t_bit_vector* bit_vector)
{
	kfree(bit_vector);
}

void bit_vector_set(t_bit_vector* bit_vector,u8 bit,index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	bit_vector[block_index] |= (1 << block_offset);
}

void bit_vector_reset(t_bit_vector* bit_vector,u8 bit,index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	bit_vector[block_index] &= (~(1 << block_offset));	
}

u8 bit_vector_get(t_bit_vector* bit_vector,index)
{
	u8 ret;
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	ret =  bit_vector[block_index] >> block_offset;
	return ret & 1;
}
