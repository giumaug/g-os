#include "general.h"
#include "memory_region/mem_regs.h"

t_mem_reg* create_mem_reg(u32 start_addr,u32 end_addr)
{
	t_mem_reg* mem_reg=NULL;

	mem_reg=kmalloc(sizeof(t_mem_reg));
	mem_reg->start_addr=start_addr;
	mem_reg->end_addr=end_addr;
	return mem_reg;
}

void delete_mem_reg(t_mem_reg* mem_reg)
{
	//static void umap_vm_mem(void* page_dir,unsigned int virt_mem_addr,unsigned int mem_size,unsigned int flush)
	kfree(mem_reg);
}

void enlarge_mem_reg()
{

}

void shrink_mem_reg()
{

}
