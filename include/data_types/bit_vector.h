#ifndef BIT_VECTOR             
#define BIT_VECTOR

#include "data_types/primitive_types.h"

typedef char t_bit_vector;

t_bit_vector* bit_vector_init(u32 size);
void bit_vector_free(t_bit_vector* bit_vector);
void bit_vector_set(t_bit_vector* bit_vector,u32 index);
void bit_vector_reset(t_bit_vector* bit_vector,u32 index);
u8 bit_vector_get(t_bit_vector* bit_vector,u32 index);

#endif
