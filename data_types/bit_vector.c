https://easwer.wordpress.com/2010/11/09/bit-vectors-in-c/

#include "memory_manager/kmalloc.h"

struct s_bit_vector
{ 	
	char* buf;
}
typedef t_bit_vector;

t_bit_vector* bit_vector_init(u32 size)
{
	t_bit_vector* bit_vector;

	bit_vector = kmalloc(sizeof(t_bit_vector));
	bit_vector->buf = kmalloc(size);
	return bit_vector;
}

void bit_vector_free(t_bit_vector* bit_vector)
{
	kfree(bit_vector->buf);
	kfree(bit_vector);
}

void bit_vector_set(t_bit_vector* bit_vector,u8 bit,index)
{
	u32 block_index;
	u32 block_offset;

	block_index = index / 8;
	block_offset = index % 8;
	[1 << block_offset;

	

}

void bit_vector_reset(t_bit_vector* bit_vector,u8 bit,index)
{

}

u8 bit_vector_get(t_bit_vector* bit_vector)
{

}

