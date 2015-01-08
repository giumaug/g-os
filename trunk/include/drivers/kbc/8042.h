#ifndef M8042_H                
#define M8042_H

#include "system.h"
#include "data_types/queue.h"

void init_kbc();
void free_kbc();
char read_buf();

#endif
